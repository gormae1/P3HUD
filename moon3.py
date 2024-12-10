from skyfield.api import load
from skyfield import almanac
from skyfield.api import N, S, E, W, load, wgs84
from skyfield.framelib import ecliptic_frame
from pytz import timezone
import datetime as dt
import sys

eph = load('de421.bsp')

zone = timezone('US/Eastern')
now = zone.localize(dt.datetime.now())

ts = load.timescale()

# if december, increase year by one
if (now.month+1)%12 == 1:
    newt = now.replace(year=now.year+1, month=1)
else:
    newt = now.replace(month=(now.month+1)%12)

t0 = ts.from_datetime(now)
t1 = ts.from_datetime(newt)
t, y = almanac.find_discrete(t0, t1, almanac.moon_phases(eph))

# find the index of the next full moon in the t array
cnt = 0
for p in y:
    if p == 2:
        break
    cnt += 1

duration = int((t0 - t[cnt]))

sun, moon, earth = eph['sun'], eph['moon'], eph['earth']

e = earth.at(t0)
s = e.observe(sun).apparent()
m = e.observe(moon).apparent()

_, slon, _ = s.frame_latlon(ecliptic_frame)
_, mlon, _ = m.frame_latlon(ecliptic_frame)
phase = (mlon.degrees - slon.degrees) % 360.0

percent = 100.0 * m.fraction_illuminated(sun)

# print(f'Phase (0°–360°): {phase}')
# print('Percent illuminated: {0:.1f}%'.format(percent))

LEEWAY = 15.0

def in_bounds(y):
    if (phase > y and phase < y + LEEWAY) or (phase < y and phase > y - LEEWAY):
        return True
    else: 
        False

select_half = -1
if in_bounds(0):
    select_half = 1
elif in_bounds(25):
    select_half = 2
elif in_bounds(45):
    select_half = 3
elif in_bounds(70):
    select_half = 4
elif in_bounds(90):
    select_half = 5
elif in_bounds(110):
    select_half = 6
elif in_bounds(135):
    select_half = 7
elif in_bounds(150):
    select_half = 8
elif in_bounds(180):
    select_half = 16
elif in_bounds(200):
    select_half = 14
elif in_bounds(225):
    select_half = 13
elif in_bounds(270):
    select_half = 12
elif in_bounds(285):
    select_half = 11
elif in_bounds(315):
    select_half = 10
elif in_bounds(340):
    select_half = 9
else:
    select_half = 1

if select_half < 10:
    sys.stdout.write("0")
    sys.stdout.write(str(select_half))
else:
    sys.stdout.write(str(select_half))

sys.stdout.write(",")

if now.day+abs(duration) < 10:
    sys.stdout.write("0")
    sys.stdout.write(str(now.day+abs(duration)))
else:
    sys.stdout.write(str(now.day+abs(duration)))
sys.stdout.write("\n")
