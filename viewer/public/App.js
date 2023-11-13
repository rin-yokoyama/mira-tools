import React, { Suspense } from 'react';

const Waveform = React.lazy(() => import('./Waveform'));
const Subtracted = React.lazy(() => import('./Subtracted'));
const FitResults = React.lazy(() => import('./FitResults'));

export function App() {
    let [showWaveform, setShowWaveform] = React.useState(false);
    let [showSubtracted, setShowSubtracted] = React.useState(false);
    let [showFitResults, setShowFitResults] = React.useState(false);

    const toggleWaveform = () => {
        setShowWaveform(!showWaveform);
    };

    const toggleSubtracted = () => {
        setShowSubtracted(!showSubtracted);
    };

    const toggleFitResults = () => {
        setShowFitResults(!showFitResults);
    };

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
            <button onClick={toggleSubtracted}>
                {showSubtracted ? 'Hide BG subtracted' : 'Show BG subtracted'}
            </button>
            {showSubtracted &&
                <Suspense fallback={<div>Loading...</div>}>
                    <Subtracted />
                </Suspense>
            }
            <br />
            <button onClick={toggleFitResults}>
                {showFitResults ? 'Hide FitResults' : 'Show FitResults'}
            </button>
            {showFitResults &&
                <Suspense fallback={<div>Loading...</div>}>
                    <FitResults />
                </Suspense>
            }

        </main>
    );
}