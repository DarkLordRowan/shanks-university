import type { z } from "zod";
import {
    ResponseSeriesSchema,
    ResponseAccelSchema,
    ResponseComputedSchema,
    ResponseEventSchema,
    ResponseErrorSchema,
    ResponseRecordSchema,
    ResponseRecordsSchema,
} from "./schema";

export type ResponseSeries = z.infer<typeof ResponseSeriesSchema>;
export type ResponseAccel = z.infer<typeof ResponseAccelSchema>;
export type ResponseComputed = z.infer<typeof ResponseComputedSchema>;
export type ResponseEvent = z.infer<typeof ResponseEventSchema>;
export type ResponseError = z.infer<typeof ResponseErrorSchema>;
export type ResponseRecord = z.infer<typeof ResponseRecordSchema>;
export type ResponseRecords = z.infer<typeof ResponseRecordsSchema>;
