import { EPS } from "./math";

export function rand01() {
    let u = Math.random();
    if (u <= 0) u = EPS;
    if (u >= 1) u = 1 - EPS;
    return u;
}

export function normal01(): number {
    const u1 = rand01();
    const u2 = rand01();
    return Math.sqrt(-2 * Math.log(u1)) * Math.cos(2 * Math.PI * u2);
}
