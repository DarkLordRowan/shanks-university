import type { Item } from "@/entities/item/model/item";
import { scoreFromItem } from "./diffUtils";

export function computeItemScore(it: Item): number | null {
    return scoreFromItem(it);
}
