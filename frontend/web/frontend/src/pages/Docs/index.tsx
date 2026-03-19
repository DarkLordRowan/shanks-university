import React, { useEffect, useMemo } from "react";
import ReactMarkdown from "react-markdown";
import { Link, useLocation } from "react-router-dom";
import remarkGfm from "remark-gfm";
import remarkMath from "remark-math";
import rehypeKatex from "rehype-katex";
import "katex/dist/katex.min.css";
import {
    buildDocsHref,
    DOCS_HOME_ANCHOR_ID,
    getDocsColumnPreferenceLabel,
    TABLE_DOCS,
    type DocsColumnSection,
    type DocsTableSection,
    type DocsXlsxFieldInfo,
    type DocsXlsxSheetInfo,
} from "@/shared/lib/docs/tableDocs";

function DocsMarkdown({ content }: { content: string }) {
    return (
        <div className="prose prose-invert max-w-none prose-headings:text-white prose-p:text-textDim prose-li:text-textDim prose-strong:text-white prose-code:text-accent prose-code:before:content-none prose-code:after:content-none prose-pre:border prose-pre:border-border/70 prose-pre:bg-surface/70 prose-a:text-primary hover:prose-a:text-accent [&_.katex-display]:overflow-x-auto [&_.katex-display]:py-2">
            <ReactMarkdown remarkPlugins={[remarkGfm, remarkMath]} rehypePlugins={[rehypeKatex]}>
                {content}
            </ReactMarkdown>
        </div>
    );
}

function isTableActive(table: DocsTableSection, activeAnchorId: string): boolean {
    return (
        activeAnchorId === table.id ||
        activeAnchorId === table.screenSectionId ||
        activeAnchorId === table.xlsxSectionId ||
        activeAnchorId.startsWith(`${table.id}-col-`) ||
        activeAnchorId.startsWith(`${table.id}-sheet-`)
    );
}

function isScreenActive(table: DocsTableSection, activeAnchorId: string): boolean {
    return activeAnchorId === table.screenSectionId || activeAnchorId.startsWith(`${table.id}-col-`);
}

function isXlsxActive(table: DocsTableSection, activeAnchorId: string): boolean {
    return activeAnchorId === table.xlsxSectionId || activeAnchorId.startsWith(`${table.id}-sheet-`);
}

function isXlsxSheetActive(sheet: DocsXlsxSheetInfo, activeAnchorId: string): boolean {
    return activeAnchorId === sheet.id || activeAnchorId.startsWith(`${sheet.id}-field-`);
}

function getTargetClasses(active: boolean): string {
    return active
        ? "border-primary/60 bg-panel ring-2 ring-primary/30 shadow-panel"
        : "border-border/70 bg-panel/80";
}

function TogglerLink({
    to,
    label,
    active,
    level = 0,
}: {
    to: string;
    label: string;
    active: boolean;
    level?: number;
}) {
    return (
        <Link
            to={to}
            className={[
                "block rounded-xl2 px-3 py-2 text-sm transition",
                level > 0 ? "ml-3 border-l border-border/70 pl-4 text-[13px]" : "",
                active
                    ? "bg-primary/15 text-white"
                    : "text-textDim hover:bg-surface/70 hover:text-white",
            ].join(" ")}
        >
            {label}
        </Link>
    );
}

function SectionCard({
    id,
    title,
    active,
    eyebrow,
    children,
}: React.PropsWithChildren<{
    id: string;
    title: string;
    active: boolean;
    eyebrow?: string;
}>) {
    return (
        <section
            id={id}
            className={[
                "scroll-mt-24 rounded-[1.5rem] border p-5 shadow-panel transition md:p-6",
                getTargetClasses(active),
            ].join(" ")}
        >
            <div className="mb-4 flex flex-wrap items-center gap-3">
                {eyebrow ? (
                    <span className="rounded-full border border-border/70 bg-surface/80 px-2.5 py-1 text-[11px] uppercase tracking-[0.18em] text-textDim">
                        {eyebrow}
                    </span>
                ) : null}
                <h2 className="text-2xl font-semibold text-white">{title}</h2>
            </div>
            {children}
        </section>
    );
}

function ColumnCard({
    column,
    active,
}: {
    column: DocsColumnSection;
    active: boolean;
}) {
    const preference = getDocsColumnPreferenceLabel(column.preference);

    return (
        <article
            id={column.id}
            className={[
                "scroll-mt-24 rounded-[1.25rem] border p-4 transition",
                active
                    ? "border-accent/70 bg-surface/90 ring-2 ring-accent/30"
                    : "border-border/70 bg-surface/60 hover:border-border",
            ].join(" ")}
        >
            <div className="mb-3 flex flex-wrap items-center gap-2">
                <h4 className="text-lg font-semibold text-white">{column.title}</h4>
                {preference ? (
                    <span className="rounded-full border border-border/70 bg-panel px-2 py-0.5 text-[11px] text-textDim">
                        {preference}
                    </span>
                ) : null}
            </div>
            <DocsMarkdown content={column.markdown} />
        </article>
    );
}

function XlsxFieldCard({
    field,
    active,
}: {
    field: DocsXlsxFieldInfo;
    active: boolean;
}) {
    const preference = field.preference ? getDocsColumnPreferenceLabel(field.preference) : null;

    return (
        <article
            id={field.id}
            className={[
                "scroll-mt-24 rounded-[1.25rem] border p-4 transition",
                active
                    ? "border-accent/70 bg-surface/90 ring-2 ring-accent/30"
                    : "border-border/70 bg-surface/60 hover:border-border",
            ].join(" ")}
        >
            <div className="mb-3 flex flex-wrap items-center gap-2">
                <h5 className="text-base font-semibold text-white">{field.name}</h5>
                {preference ? (
                    <span className="rounded-full border border-border/70 bg-panel px-2 py-0.5 text-[11px] text-textDim">
                        {preference}
                    </span>
                ) : null}
                {field.refAnchorId ? (
                    <Link
                        to={buildDocsHref(field.refAnchorId)}
                        className="rounded-full border border-primary/30 bg-primary/10 px-2 py-0.5 text-[11px] text-primary transition hover:border-primary/50 hover:text-white"
                    >
                        Экранная метрика
                    </Link>
                ) : null}
            </div>
            <DocsMarkdown content={field.markdown} />
        </article>
    );
}

const DocsPage: React.FC = () => {
    const location = useLocation();

    const activeAnchorId = useMemo(() => {
        const hash = location.hash.startsWith("#")
            ? decodeURIComponent(location.hash.slice(1))
            : "";
        return hash || DOCS_HOME_ANCHOR_ID;
    }, [location.hash]);

    useEffect(() => {
        const hash = location.hash.startsWith("#")
            ? decodeURIComponent(location.hash.slice(1))
            : "";
        if (!hash) return;

        let frame = 0;
        let frame2 = 0;
        frame = window.requestAnimationFrame(() => {
            frame2 = window.requestAnimationFrame(() => {
                const element = document.getElementById(hash);
                if (!element) return;
                element.scrollIntoView({ behavior: "smooth", block: "start" });
            });
        });

        return () => {
            window.cancelAnimationFrame(frame);
            window.cancelAnimationFrame(frame2);
        };
    }, [location.hash, location.pathname]);

    const aliasPath = location.pathname !== "/docs";

    return (
        <div className="space-y-6">
            <section
                id={DOCS_HOME_ANCHOR_ID}
                className={[
                    "scroll-mt-24 overflow-hidden rounded-[2rem] border border-border/70 bg-panel shadow-panel",
                    activeAnchorId === DOCS_HOME_ANCHOR_ID ? "ring-2 ring-primary/30" : "",
                ].join(" ")}
            >
                <div className="bg-[radial-gradient(circle_at_top_left,_rgba(59,130,246,0.18),_transparent_42%),radial-gradient(circle_at_top_right,_rgba(6,182,212,0.18),_transparent_36%),linear-gradient(180deg,rgba(15,23,42,0.9),rgba(15,23,42,0.78))] p-6 md:p-8">
                    <div className="mb-3 flex flex-wrap items-center gap-3">
                        <span className="rounded-full border border-primary/30 bg-primary/10 px-3 py-1 text-xs font-medium uppercase tracking-[0.18em] text-primary">
                            Документация v1
                        </span>
                        <span className="rounded-full border border-border/70 bg-surface/70 px-3 py-1 text-xs text-textDim">
                            Экран + XLSX
                        </span>
                    </div>

                    <div className="grid gap-6 lg:grid-cols-[minmax(0,1fr)_300px]">
                        <div className="space-y-4">
                            <h1 className="max-w-4xl text-3xl font-semibold text-white md:text-4xl">
                                Документация по таблицам анализа рядов и алгоритмов
                            </h1>
                            <p className="max-w-3xl text-sm leading-7 text-textDim md:text-base">
                                Эта страница описывает, как читать экранные таблицы и их xlsx-экспорт.
                                Кнопка <span className="font-semibold text-white">?</span> рядом с
                                заголовком таблицы, колонки или ключевого элемента ведёт сразу к нужному разделу.
                            </p>
                            <div className="grid gap-3 md:grid-cols-3">
                                <div className="rounded-[1.25rem] border border-border/70 bg-surface/60 p-4">
                                    <div className="text-xs uppercase tracking-[0.18em] text-textDim/80">
                                        Что покрыто
                                    </div>
                                    <div className="mt-2 text-sm text-white">
                                        3 таблицы, экранные элементы, xlsx-листы и формулы.
                                    </div>
                                </div>
                                <div className="rounded-[1.25rem] border border-border/70 bg-surface/60 p-4">
                                    <div className="text-xs uppercase tracking-[0.18em] text-textDim/80">
                                        Формулы
                                    </div>
                                    <div className="mt-2 text-sm text-white">
                                        Все основные метрики записаны в явном виде через LaTeX.
                                    </div>
                                </div>
                                <div className="rounded-[1.25rem] border border-border/70 bg-surface/60 p-4">
                                    <div className="text-xs uppercase tracking-[0.18em] text-textDim/80">
                                        Навигация
                                    </div>
                                    <div className="mt-2 text-sm text-white">
                                        Якоря едины для UI и документации, поэтому ссылки не расходятся.
                                    </div>
                                </div>
                            </div>
                            {aliasPath ? (
                                <div className="rounded-[1.25rem] border border-accent/30 bg-accent/10 p-4 text-sm text-textDim">
                                    Маршрут вида <code>/docs/manuals/*</code> сейчас ведёт на эту общую
                                    страницу документации. Для точного раздела используй якоря справа или
                                    кнопки <span className="font-semibold text-white">?</span> в таблицах.
                                </div>
                            ) : null}
                        </div>

                        <aside className="rounded-[1.5rem] border border-border/70 bg-surface/60 p-4">
                            <div className="mb-3 text-xs uppercase tracking-[0.18em] text-textDim/80">
                                Как читать страницу
                            </div>
                            <ul className="space-y-2 text-sm text-textDim">
                                <li>Сначала идут общая идея таблицы и правила чтения экранной версии.</li>
                                <li>Дальше отдельными карточками разобраны все колонки и фиксированные элементы интерфейса.</li>
                                <li>В конце каждой секции описан xlsx-экспорт и его листы.</li>
                            </ul>
                        </aside>
                    </div>
                </div>
            </section>

            <div className="grid gap-6 xl:grid-cols-[minmax(0,1fr)_300px]">
                <div className="space-y-6">
                    {TABLE_DOCS.map((table) => (
                        <div key={table.key} className="space-y-4">
                            <SectionCard
                                id={table.id}
                                title={table.title}
                                eyebrow="Таблица"
                                active={isTableActive(table, activeAnchorId)}
                            >
                                <div className="space-y-4">
                                    <p className="max-w-4xl text-sm leading-7 text-textDim md:text-base">
                                        {table.summary}
                                    </p>
                                    <DocsMarkdown content={table.introMarkdown} />
                                </div>
                            </SectionCard>

                            <SectionCard
                                id={table.screenSectionId}
                                title="Экранная таблица"
                                eyebrow="UI"
                                active={isScreenActive(table, activeAnchorId)}
                            >
                                <div className="space-y-5">
                                    <DocsMarkdown content={table.screenMarkdown} />

                                    <div>
                                        <div className="mb-3 text-xs uppercase tracking-[0.18em] text-textDim/80">
                                            Быстрые ссылки по колонкам и элементам
                                        </div>
                                        <div className="flex flex-wrap gap-2">
                                            {table.columns.map((column) => (
                                                <Link
                                                    key={column.id}
                                                    to={buildDocsHref(column.id)}
                                                    className={[
                                                        "rounded-full border px-3 py-1.5 text-xs transition",
                                                        activeAnchorId === column.id
                                                            ? "border-primary/50 bg-primary/10 text-white"
                                                            : "border-border/70 bg-surface/70 text-textDim hover:border-primary/40 hover:text-white",
                                                    ].join(" ")}
                                                >
                                                    {column.title}
                                                </Link>
                                            ))}
                                        </div>
                                    </div>

                                    <div className="grid gap-4 xl:grid-cols-2">
                                        {table.columns.map((column) => (
                                            <ColumnCard
                                                key={column.id}
                                                column={column}
                                                active={activeAnchorId === column.id}
                                            />
                                        ))}
                                    </div>
                                </div>
                            </SectionCard>

                            <SectionCard
                                id={table.xlsxSectionId}
                                title="XLSX-экспорт"
                                eyebrow="XLSX"
                                active={isXlsxActive(table, activeAnchorId)}
                            >
                                <div className="space-y-5">
                                    <DocsMarkdown content={table.xlsxMarkdown} />

                                    <div className="grid gap-4 md:grid-cols-2">
                                        {table.xlsxSheets.map((sheet) => (
                                            <article
                                                key={sheet.name}
                                                id={sheet.id}
                                                className={[
                                                    "scroll-mt-24 rounded-[1.25rem] border p-4 transition",
                                                    isXlsxSheetActive(sheet, activeAnchorId)
                                                        ? "border-accent/70 bg-surface/90 ring-2 ring-accent/30"
                                                        : "border-border/70 bg-surface/60",
                                                ].join(" ")}
                                            >
                                                <div className="mb-2 flex items-center justify-between gap-3">
                                                    <h4 className="text-lg font-semibold text-white">
                                                        {sheet.name}
                                                    </h4>
                                                    <span className="rounded-full border border-border/70 bg-panel px-2 py-0.5 text-[11px] text-textDim">
                                                        Лист
                                                    </span>
                                                </div>
                                                <p className="mb-3 text-sm text-textDim">
                                                    {sheet.description}
                                                </p>
                                                <DocsMarkdown content={sheet.markdown} />

                                                {sheet.fields.length > 0 ? (
                                                    <div className="mt-4 space-y-3">
                                                        <div className="text-xs uppercase tracking-[0.18em] text-textDim/80">
                                                            Поля листа
                                                        </div>
                                                        <div className="grid gap-3">
                                                            {sheet.fields.map((field) => (
                                                                <XlsxFieldCard
                                                                    key={field.id}
                                                                    field={field}
                                                                    active={activeAnchorId === field.id}
                                                                />
                                                            ))}
                                                        </div>
                                                    </div>
                                                ) : null}
                                            </article>
                                        ))}
                                    </div>
                                </div>
                            </SectionCard>
                        </div>
                    ))}
                </div>

                <aside className="h-fit xl:sticky xl:top-20">
                    <div className="rounded-[1.5rem] border border-border/70 bg-panel/85 p-4 shadow-panel">
                        <div className="mb-3 text-xs uppercase tracking-[0.18em] text-textDim/80">
                            Навигация
                        </div>
                        <nav className="space-y-1">
                            <TogglerLink
                                to={buildDocsHref(DOCS_HOME_ANCHOR_ID)}
                                label="Обзор"
                                active={activeAnchorId === DOCS_HOME_ANCHOR_ID}
                            />
                            {TABLE_DOCS.map((table) => (
                                <div key={table.key} className="space-y-1">
                                    <TogglerLink
                                        to={buildDocsHref(table.id)}
                                        label={table.title}
                                        active={isTableActive(table, activeAnchorId)}
                                    />
                                    <TogglerLink
                                        to={buildDocsHref(table.screenSectionId)}
                                        label="Экранная таблица"
                                        level={1}
                                        active={isScreenActive(table, activeAnchorId)}
                                    />
                                    <TogglerLink
                                        to={buildDocsHref(table.xlsxSectionId)}
                                        label="XLSX-экспорт"
                                        level={1}
                                        active={activeAnchorId === table.xlsxSectionId}
                                    />
                                </div>
                            ))}
                        </nav>
                    </div>
                </aside>
            </div>
        </div>
    );
};

export default DocsPage;
