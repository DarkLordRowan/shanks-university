import { z } from "zod";

export const ResponseSeriesSchema = z.object({
    name: z.string(),
    arguments: z.object({x: z.number()}),
    lim: z.number().nullable().optional(),
});

export const ResponseAccelSchema = z.object({
    name: z.string(),
    m_value: z.number(),
    additional_args: z
        .object({
            remainder: z.string().optional(),
            useRecFormulas: z.string().optional(),
            beta: z.string().optional(),
        })
        .nullable(),
});

export const ResponseComputedSchema = z.object({
    n: z.number(),
    series_value: z.number().nullable(),
    partial_sum: z.number().nullable(),
    partial_sum_deviation: z.number().nullable(),
    accel_value: z.number().nullable(),
    accel_value_deviation: z.number().nullable(),
});

export const ResponseErrorSchema = z.object({
    description: z.string(),
    data: z.object({n: z.number().nullable()}),
});

export const ResponseRecordSchema = z.object({
    id: z.string(),
    series: ResponseSeriesSchema,
    accel: ResponseAccelSchema,
    computed: z.array(ResponseComputedSchema),
    error: ResponseErrorSchema.nullable(),
});

export const ResponseRecordsSchema = z.array(ResponseRecordSchema);

export type ResponseSeries = z.infer<typeof ResponseSeriesSchema>;
export type ResponseAccel = z.infer<typeof ResponseAccelSchema>;
export type ResponseComputed = z.infer<typeof ResponseComputedSchema>;
export type ResponseError = z.infer<typeof ResponseErrorSchema>;
export type ResponseRecord = z.infer<typeof ResponseRecordSchema>;

export function parseResponse(json: unknown): ResponseRecord[] {
    return ResponseRecordsSchema.parse(json);
}