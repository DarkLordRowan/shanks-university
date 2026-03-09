export interface ChunkProgressState {
    running: boolean;
    current: number;
    total: number;
}

interface RunChunkedWithProgressOptions {
    total: number;
    chunkSize: number;
    isCancelled: () => boolean;
    onRange: (start: number, end: number) => void;
    onProgress: (state: ChunkProgressState) => void;
    onDone: () => void;
}

export function runChunkedWithProgress(options: RunChunkedWithProgressOptions): void {
    const { total, chunkSize, isCancelled, onRange, onProgress, onDone } = options;
    const step = Math.max(1, chunkSize);

    function processChunk(start: number) {
        if (isCancelled()) return;

        const end = Math.min(start + step, total);
        onRange(start, end);

        const running = end < total;
        onProgress({ running, current: end, total });

        if (running) {
            setTimeout(() => processChunk(end), 0);
            return;
        }

        if (!isCancelled()) {
            onDone();
        }
    }

    if (total <= 0) {
        onProgress({ running: false, current: 0, total });
        if (!isCancelled()) {
            onDone();
        }
        return;
    }

    processChunk(0);
}
