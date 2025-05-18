import {type ChangeEvent, useEffect, useState} from 'react';

interface SeriesComputeResponse {
    sum: number;
    term: number;
}

interface AccelComputeResponse {
    accelerated: number;
    difference: number;
}

export default function App() {
    const [seriesList, setSeriesList] = useState<string[]>([]);
    const [accelList, setAccelList] = useState<string[]>([]);

    const [mode, setMode] = useState<'series' | 'acceleration'>('series');

    const [params, setParams] = useState({
        series: '',
        x: 0,
        n: 1,
        accelerator: '',
        order: 1,
    });

    const [result, setResult] = useState<SeriesComputeResponse | AccelComputeResponse | null>(null);

    useEffect(() => {
        fetch('http://localhost:8000/api/series')
            .then(res => res.json())
            .then(data => setSeriesList(Object.keys(data)))
            .catch(err => console.error("Failed to fetch series list", err));
        fetch('http://localhost:8000/api/accelerations')
            .then(res => res.json())
            .then(data => setAccelList(Object.keys(data)))
            .catch(err => console.error("Failed to fetch acceleration list", err));
    }, []);

    const handleChange = (e: ChangeEvent<HTMLInputElement | HTMLSelectElement>) => {
        setParams(prev => ({ ...prev, [e.target.name]: e.target.type === 'number' ? Number(e.target.value) : e.target.value }));
    };

    const handleCompute = async () => {
        const url = mode === 'series'
            ? 'http://localhost:8000/api/series/compute'
            : 'http://localhost:8000/api/acceleration/compute';
        const body = mode === 'series'
            ? { series: params.series, x: params.x, n: params.n }
            : { series: params.series, accelerator: params.accelerator, x: params.x, n: params.n, order: params.order };

        try {
            const res = await fetch(url, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify(body),
            });
            if (!res.ok) {
                const errorText = await res.text();
                throw new Error(errorText);
            }
            const data = await res.json();
            setResult(data);
        } catch (error) {
            console.error("Compute error:", error);
            setResult(null);
        }
    };

    return (
        <div className="p-4 max-w-md mx-auto">
            <h1 className="text-xl font-bold mb-4">Series Computation</h1>
            <div className="flex mb-4">
                <button onClick={() => setMode('series')} className={mode === 'series' ? 'font-semibold' : ''}>Series</button>
                <button onClick={() => setMode('acceleration')} className={mode === 'acceleration' ? 'font-semibold ml-4' : 'ml-4'}>Acceleration</button>
            </div>
            <div className="space-y-2">
                <div>
                    <label className="block">Series:</label>
                    <select name="series" value={params.series} onChange={handleChange}>
                        <option value="">Select</option>
                        {seriesList.map(s => <option key={s} value={s}>{s}</option>)}
                    </select>
                </div>
                <div>
                    <label className="block">x:</label>
                    <input name="x" type="number" value={params.x} onChange={handleChange} />
                </div>
                <div>
                    <label className="block">n:</label>
                    <input name="n" type="number" value={params.n} onChange={handleChange} min={1} />
                </div>
                {mode === 'acceleration' && (
                    <>
                        <div>
                            <label className="block">Accelerator:</label>
                            <select name="accelerator" value={params.accelerator} onChange={handleChange}>
                                <option value="">Select</option>
                                {accelList.map(a => <option key={a} value={a}>{a}</option>)}
                            </select>
                        </div>
                        <div>
                            <label className="block">Order:</label>
                            <input name="order" type="number" value={params.order} onChange={handleChange} min={1} />
                        </div>
                    </>
                )}
            </div>
            <button onClick={handleCompute} className="mt-4 px-4 py-2 bg-blue-600 text-white rounded">Compute</button>
            {result && (
                <pre className="mt-4 bg-gray-100 p-2 rounded">{JSON.stringify(result, null, 2)}</pre>
            )}
        </div>
    );
}