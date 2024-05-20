import React, { Suspense } from 'react';

const Waveform = React.lazy(() => import('./Waveform'));
//const FitResults = React.lazy(() => import('./FitResults'));

export function App() {
    let [showWaveform, setShowWaveform] = React.useState(false);
    //let [showFitResults, setShowFitResults] = React.useState(false);

    const toggleWaveform = () => {
        setShowWaveform(!showWaveform);
    };

    //const toggleFitResults = () => {
    //    setShowFitResults(!showFitResults);
    //};

    return (
        <main>
            <h1>mira-viewer home</h1>
            <button onClick={toggleWaveform}>
                {showWaveform ? 'Hide Waveform' : 'Show Waveform'}
            </button>
            {showWaveform &&
                <Suspense fallback={<div>Loading...</div>}>
                    <Waveform />
                </Suspense>
            }
            <br />

        </main>
    );
}