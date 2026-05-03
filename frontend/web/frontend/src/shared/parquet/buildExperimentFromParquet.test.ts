import { describe, expect, it } from "vitest";
import type { Series } from "@/entities/experiment/model/experiment";
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
});
