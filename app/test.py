import unittest
from pathlib import Path
import app


class TestApp(unittest.TestCase):

    def test_extract_timestamp_and_filename(self):
        test_cases = [
            (Path("1639591954702392600_6144x2048.rgb"), (1639591954702392600, Path("6144x2048.rgb"))),
            (Path("path/to/1639591954702392600_6144x2048.rgb"), (1639591954702392600, Path("path/to/6144x2048.rgb"))),
            (Path("/path/to/1639591954702392600_6144x2048.rgb"), (1639591954702392600, Path("/path/to/6144x2048.rgb"))),
            (Path("1639581754702392600_right_336x252_14bit.thermal.raw"), (1639581754702392600, Path("right_336x252_14bit.thermal.raw"))),
            (Path("1639581754702392600_no_suffix"), (1639581754702392600, Path("no_suffix"))),
        ]

        for input, expect in test_cases:
            self.assertEqual(app.extract_timestamp_and_filename(input), expect)


    def test_extract_resolution(self):
        test_cases = [
            (Path("1639591954702392600_6144x2048.rgb"), "6144x2048"),
            (Path("1639591954702392600_1280x800.rgb"), "1280x800"),
            (Path("1639591954702392600_64x64.rgb"), "64x64"),
            (Path("path/to/1639591954702392600_6144x2048.rgb"), "6144x2048"),
            (Path("/path/to/1639591954702392600_6144x2048.rgb"), "6144x2048"),
        ]

        for input, expect in test_cases:
            self.assertEqual(app.extract_resolution(input), expect)


if __name__ == "__main__":
    unittest.main()
