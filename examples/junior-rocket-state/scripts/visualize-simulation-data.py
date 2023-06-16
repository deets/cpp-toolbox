# Copyright: 2023, Diez B. Roggisch, Berlin . All rights reserved.
import sys
from collections import deque
import math

import scipy.optimize
import pandas as pd
import numpy as np
import plotly.graph_objects as go
from plotly.subplots import make_subplots

pd.options.plotting.backend = "plotly"


OR_EXPORT_HEADERS = [
    "time",
    #"altitude",
    #"vacc",
    #"speed",
    "totalacc",
    # "lateralacc",
    # "roll",
    # "pitch",
    # "yaw",
    "pressure",
    ]

BURNOUT = 2.242

def parabola(x, a, b, c):
    return a*x**2 + b*x + c


def parabola_detection(points):
    times, pressures = zip(*points)
    fit_params, pcov = scipy.optimize.curve_fit(parabola, times, pressures)
    quality = np.sqrt(np.diag(pcov))
    return fit_params, quality


def main():
    first_stage = pd.read_csv(
        sys.argv[1],
        comment="#",
        names=OR_EXPORT_HEADERS,
    )
    second_stage = pd.read_csv(
        sys.argv[2],
        comment="#",
        names=OR_EXPORT_HEADERS,
    )
    # The second stage file contains *all* data,
    # the first stage one only the rest of the first
    # stage's flight. Thus first stage is the file
    # with a timestamp greater than second stage.
    if first_stage["time"][0] < second_stage["time"][0]:
        first_stage, second_stage = second_stage, first_stage

    early_first_stage = second_stage[second_stage["time"] < first_stage["time"][0]]
    full_first_stage = pd.concat([early_first_stage, first_stage])

    points = deque(maxlen=30)
    min_quality = None
    qualities = []
    for index, row in full_first_stage.iterrows():
        if row.time > BURNOUT:
            points.append((row.time, row.pressure))
            if len(points) == 30:
                params, quality = parabola_detection(points)
                quality = np.sum(quality**2)
                qualities.append(quality)
                if min_quality is None or quality < min_quality:
                    min_quality = quality
                    a, b, c = params
                    parabola_times = np.linspace(points[0][0], points[0][0] + 5.0, 100)
                    parabola_points = [parabola(x, a, b, c) for x in parabola_times]
            else:
                qualities.append(math.nan)

    #full_first_stage['p_q'] = pd.Series(qualities, index=full_first_stage.index)
    trace1 = go.Line(
        x=full_first_stage.time,
        y=full_first_stage.pressure,
        name="pressure",
    )
    # trace2 = go.Line(
    #     x=full_first_stage.time,
    #     y=full_first_stage.p_q,
    #     name='Quality',
    #     yaxis='y2'
    # )
    fig = make_subplots(specs=[[{"secondary_y": True}]])
    fig.add_trace(trace1)
    trace3 = go.Line(
        x=parabola_times,
        y=parabola_points,
        name='parabola',
    )
    fig.add_trace(trace3)
    #fig.add_trace(trace2, secondary_y=True)
    fig.show()

# main guard
if __name__ == '__main__':
    main()
