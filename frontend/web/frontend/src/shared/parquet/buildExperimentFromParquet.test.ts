import { describe, expect, it } from "vitest";
import type { Series } from "@/entities/experiment/model/experiment";
import { buildErrorMatrixFromExperiment } from "@/shared/lib/error-matrix/buildErrorMatrix";
import type { ParquetAccelRow } from "./types";
import { buildAccelAndSeriesAccelEntityVariantsFromParquetRow } from "./buildExperimentFromParquet";

describe("buildAccelAndSeriesAccelEntityVariantsFromParquetRow", () => {
    it("creates a synthetic filtered algorithm for each filtered method", () => {
        const series: Series = {
            id: "7",
            name: "S",
            precision: "f64",
            args: null,
            limit: { re: 10, im: 0 },
            computed: [],
        };
        const row: ParquetAccelRow = {
            series_id: 7,
            accel_name: "Aitken",
            m_value: 2,
            additional_args: { beta: "0.5" },
            computed: [],
            errors: [],
            events: [],
            filtered: {
                start_n: 0,
                segment_length: 2,
                filter_args: {
                    degree: "2",
                    window_length: "3",
                },
                methods: {
                    "Kolmogorov-Zurbenko": {
                        values: [
                            { real: "9", imag: "0" },
                            { real: "10", imag: "0" },
                        ],
                        average: { real: "9.5", imag: "0" },
                    },
                },
            },
        };

        const variants = buildAccelAndSeriesAccelEntityVariantsFromParquetRow({
            row,
            series,
        });

        expect(variants).toHaveLength(2);
        expect(variants[0].accel.variant).toBe("raw");
        expect(variants[0].seriesAccel.variant).toBe("raw");

        const filtered = variants[1];
        expect(filtered.accel.variant).toBe("filtered");
        expect(filtered.accel.name).toBe("Aitken");
        expect(filtered.accel.baseAccelId).toBe(variants[0].accel.id);
        expect(filtered.accel.filteredMethodName).toBe(
            "Kolmogorov-Zurbenko: degree=2, window_length=3"
        );
        expect(filtered.accel.args).toMatchObject({
            beta: "0.5",
            filtered: true,
            filter_method: "Kolmogorov-Zurbenko",
            filter_degree: "2",
            filter_window_length: "3",
        });

        expect(filtered.seriesAccel.variant).toBe("filtered");
        expect(filtered.seriesAccel.baseAccelId).toBe(variants[0].accel.id);
        expect(filtered.seriesAccel.filteredMethodName).toBe(
            "Kolmogorov-Zurbenko: degree=2, window_length=3"
        );
        expect(filtered.seriesAccel.filteredStartN).toBe(0);
        expect(filtered.seriesAccel.filteredSegmentLength).toBe(2);
        expect(filtered.seriesAccel.filtered).toBeNull();
        expect(filtered.seriesAccel.computed).toEqual([
            {
                n: 1,
                value: { re: 9, im: 0 },
                deviation: 1,
                profiling: null,
            },
            {
                n: 2,
                value: { re: 10, im: 0 },
                deviation: 0,
                profiling: null,
            },
        ]);
    });

    it("normalizes error messages and steps from alternate parquet fields", () => {
        const series: Series = {
            id: "7",
            name: "S",
            precision: "f64",
            args: null,
            limit: { re: 10, im: 0 },
            computed: [],
        };
        const row: ParquetAccelRow = {
            series_id: 7,
            accel_name: "Aitken",
            m_value: 2,
            additional_args: null,
            computed: [
                { n: 0, value: null, deviation: null },
                { n: 2, value: null, deviation: null },
                { n: 3, value: null, deviation: null },
            ],
            errors: [
                { n: 0, message: "not enough sum" },
                { computed_index: 2, description: "division by zero" },
                { data: { computed_index: 3, description: "nested division by zero" } },
                { description: "unknown step" },
            ],
            events: [],
        };

        const [variant] = buildAccelAndSeriesAccelEntityVariantsFromParquetRow({
            row,
            series,
        });

        expect(variant.seriesAccel.computed.map((point) => point.n)).toEqual([0, 2, 3]);
        expect(variant.seriesAccel.errors).toEqual([
            { n: 0, message: "not enough sum" },
            { n: 2, message: "division by zero" },
            { n: 3, message: "nested division by zero" },
            { n: null, message: "unknown step" },
        ]);

        const matrix = buildErrorMatrixFromExperiment(
            {
                id: "exp",
                seriesList: [series],
                accelList: [variant.accel],
                seriesAccelList: [variant.seriesAccel],
            },
            null
        );

        expect(matrix.nList).toEqual([0, 2, 3, null]);
        expect(matrix.totalErrorItems).toBe(4);
        expect(matrix.cellMap.get(`${variant.accel.id}||unknown`)).toBe(1);
    });
});
