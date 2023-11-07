import React, { Suspense } from 'react';

const Waveform = React.lazy(() => import('./Waveform'));
export function App() {
    let [showWaveform, setShowWaveform] = React.useState(false);
    //let [showFitResults, setShowFitResults] = React.useState(false);

    return (
        <main>
            <h1>mira-viewer home</h1>
            <button onClick={() => setShowWaveform(true)}>Waveform</button>
            {showWaveform &&
                <Suspense fallback={<div>Loading...</div>}>
                    <Waveform />
                </Suspense>
            }
        </main>
    );
}