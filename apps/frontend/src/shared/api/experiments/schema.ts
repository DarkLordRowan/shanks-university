import { z } from "zod";

export const NumOrStr = z.union([z.number(), z.string()]).nullable().optional();

export const ResponseSeriesSchema = z
    .object({
        name: z.string(),
        arguments: z
            .object({
                x: z.number().nullable().optional(),
            })
            .loose(),
        lim: NumOrStr,
    })
    .loose();

export const ResponseAccelSchema = z
    .object({
        name: z.string(),
        m_value: z.number().nullable().optional(),
        additional_args: z
            .object({
                epsilon_threshold: z.string().optional(),
            })
            .loose()
            .nullable()
            .optional(),
    })
    .loose();

export const ResponseComputedSchema = z
    .object({
        n: z.number(),
        series_value: NumOrStr,
        partial_sum: NumOrStr,
        partial_sum_deviation: NumOrStr,
        accel_value: NumOrStr,
        accel_value_deviation: NumOrStr,
    })
    .loose();

export const ResponseEventDataSchema = z
    .object({
        computed_index: z.number().optional(),
        description: z.string().optional(),
    })
    .loose();

export const ResponseEventSchema = z
    .object({
        event: z.string(),
        data: ResponseEventDataSchema.optional(),
    })
    .loose();

export const ResponseErrorSchema = z
    .object({
        description: z.string(),
        data: z
            .record(z.string(), z.unknown())
            .optional(),
    })
    .loose();

export const ResponseRecordSchema = z
    .object({
        stack_id: z.string().optional(),
        series: ResponseSeriesSchema,
        accel: ResponseAccelSchema,
        computed: z.array(ResponseComputedSchema),
        error: ResponseErrorSchema.nullable().optional(),
        events: z.array(ResponseEventSchema).optional().default([]),
    })
    .loose();

export const ResponseRecordsSchema = z.array(ResponseRecordSchema);
