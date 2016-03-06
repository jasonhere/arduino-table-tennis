### Variables

- total wins <b>w</b>
- total loses <b>l</b>
- total games <b>t = w + l </b>
- coeffiecient <b>k = w / t</b>

##### Initial rating
 
 - average opponents rating <b>a</b>
 - initial rating <b>k * a + 0,46 * a | Coeffiecient * average opponents rating + 0.46 * average opponents rating</b>
 
##### Rating change
<b>(opponent's rating - my rating + 1000) / c</b>
 - c depends on total games (t).
   - 6 -> 12 games: c = 28
   - 13 -> 25: c = 60
   - 26 -> 51: c = 88
   - 52 -> 100: c = 116
   - 101 -> 200: c = 144
   - 201 -> 500: c = 172
   - 500+: c = 200
