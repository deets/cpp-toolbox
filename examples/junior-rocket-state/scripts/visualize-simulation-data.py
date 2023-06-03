# Copyright: 2023, Diez B. Roggisch, Berlin . All rights reserved.
import sys

import pandas as pd
import matplotlib.pyplot as plt


OR_EXPORT_HEADERS = [
    "time",
    "altitude",
    #"vacc",
    #"speed",
    "totalacc",
    # "lateralacc",
    # "roll",
    # "pitch",
    # "yaw",
    "pressure",
    ]

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
    print(first_stage["time"][0])
    early_first_stage = second_stage[second_stage["time"] < first_stage["time"][0]]
    full_first_stage = pd.concat([early_first_stage, first_stage])
    full_first_stage.plot(x="time", y="totalacc")
    full_first_stage.plot(x="time", y="pressure")
    full_first_stage.plot(x="time", y="altitude")
    plt.show()


# main guard
if __name__ == '__main__':
    main()
