import type { SeriesNode } from "../data/series";
import { Formula } from "./Formula";
import { Combobox, type ComboboxProps, ComboboxRow } from "./ui/Combobox";

export type SelectSeriesProps = Pick<
    ComboboxProps<SeriesNode>,
    "items" | "value" | "onChange" | "className" | "placeholder"
> & {
    searchField?: keyof Pick<SeriesNode, "title" | "formula">;
};

export function SelectSeries({
                                 items,
                                 value,
                                 onChange,
                                 className,
                                 placeholder = "Выберите ряд…",
                                 searchField = "title",
                             }: SelectSeriesProps) {
    return (
        <Combobox<SeriesNode>
            items={items}
            value={value}
            onChange={onChange}
            className={className}
            placeholder={placeholder}
            getKey={(x) => x.id}
            getSearchText={(x) => {
                const base = `${x.title} ${x.id} ${x.python_id} ${x.formula ?? ""}`;
                return searchField === "formula" ? `${x.formula ?? ""} ${base}` : base;
            }}
            renderOption={(x, {selected}) => (
                <ComboboxRow
                    left={
                        <span className="inline-flex items-baseline gap-1">
                            {x.num}. <Formula latex={x.title}/>
                        </span>
                    }
                    right={selected ? <span className="ml-2 text-xs text-primary">выбрано</span> : null}
                    sub={x.python_id}
                />
            )}
            renderSelected={(x) => (
                <ComboboxRow
                    left={
                        <span className="inline-flex items-baseline gap-1">
                            {x.num}. <Formula latex={x.title}/>
                        </span>
                    }
                    right={<span className="ml-2 text-xs text-primary">выбрано</span>}
                    sub={x.python_id}
                />
            )}
        />
    );
}
