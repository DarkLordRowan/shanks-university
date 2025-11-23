// src/widgets/ExperimentViewer/ExperimentViewer.tsx

import type { Experiment, Series, Accel, SeriesAccel } from "@/entities/experiment/model/experiment";

interface ExperimentViewerProps {
    experiment: Experiment | null;
}

function printSeries(s: Series): string {
    return [
        `  id: ${s.id}`,
        `  name: ${s.name}`,
        `  precision: ${s.precision}`,
        `  args: ${s.args ? JSON.stringify(s.args) : "null"}`,
        `  limit: ${
            s.limit
                ? `{ re: ${s.limit.re}, im: ${s.limit.im} }`
                : "null"
        }`,
    ].join("\n");
}

function printAccel(a: Accel): string {
    return [
        `  id: ${a.id}`,
        `  name: ${a.name}`,
        `  m: ${a.m}`,
        `  args: ${a.args ? JSON.stringify(a.args) : "null"}`,
    ].join("\n");
}

function printSeriesAccel(sa: SeriesAccel): string {
    const computed = sa.computed
        .map(
            (c) =>
                `    n=${c.n}, value=${
                    c.value
                        ? `{ re: ${c.value.re}, im: ${c.value.im} }`
                        : "null"
                }, deviation=${c.deviation ?? "null"}`
        )
        .join("\n");

    const errors = sa.errors
        .map(
            (e) =>
                `    n=${e.n}, message="${e.message}"`
        )
        .join("\n");

    const events = sa.events
        .map(
            (ev) =>
                `    n=${ev.n}, name="${ev.name}", description="${ev.description}"`
        )
        .join("\n");

    return [
        `  series_id: ${sa.series_id}`,
        `  accel_id: ${sa.accel_id}`,
        `  computed:\n${computed === "" ? "    <empty>" : computed}`,
        `  errors:\n${errors === "" ? "    <empty>" : errors}`,
        `  events:\n${events === "" ? "    <empty>" : events}`,
    ].join("\n");
}

export function ExperimentViewer({ experiment }: ExperimentViewerProps) {
    if (!experiment) {
        return (
            <pre className="text-sm text-textDim">
                experiment = null
            </pre>
        );
    }

    const lines: string[] = [];

    // seriesList
    lines.push("seriesList:");
    if (!experiment.seriesList || experiment.seriesList.length === 0) {
        lines.push("  <empty>");
    } else {
        experiment.seriesList.forEach((s, i) => {
            lines.push(`- series[${i}]`);
            lines.push(printSeries(s));
        });
    }
    lines.push("");

    // accelList
    lines.push("accelList:");
    if (!experiment.accelList || experiment.accelList.length === 0) {
        lines.push("  <empty>");
    } else {
        experiment.accelList.forEach((a, i) => {
            lines.push(`- accel[${i}]`);
            lines.push(printAccel(a));
        });
    }
    lines.push("");

    // seriesAccelList
    lines.push("seriesAccelList:");
    if (!experiment.seriesAccelList || experiment.seriesAccelList.length === 0) {
        lines.push("  <empty>");
    } else {
        experiment.seriesAccelList.forEach((sa, i) => {
            lines.push(`- seriesAccel[${i}]`);
            lines.push(printSeriesAccel(sa));
        });
    }

    const text = lines.join("\n");

    return (
        <div className="p-4 border border-border rounded-md overflow-auto max-h-[80vh]">
            <pre className="text-xs leading-snug whitespace-pre-wrap break-all">
                {text}
            </pre>
        </div>
    );
}
