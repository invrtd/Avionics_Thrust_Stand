# Thrust Stand NMEA Protocol

The Thrust Stand streams telemetry data over USB Serial and Bluetooth Serial using a standard NMEA 0183 sentence structure.

## General Format
`$TYPE,Field1,Field2,...,Timestamp*CS`

- **$**: Start of sentence.
- **TYPE**: Sentence identifier (e.g., RPM, THRUST).
- **Fields**: Comma-separated data values.
- **Timestamp**: System time in milliseconds (`millis()`).
- **\*CS**: Checksum (XOR of all characters between `$` and `*`), formatted as a 2-digit hex value.
- **\r\n**: End of sentence.

---

## Sentences

### 1. RPM
Engine rotation speed.
`$RPM,CurrentRPM,TotalRevs,Timestamp*CS`

| Field | Description | Unit | Example |
|---|---|---|---|
| CurrentRPM | Real-time rotation speed | RPM | `1250` |
| TotalRevs | Cumulative total revolutions | Revs | `15.5` |

### 2. Thrust
Load cell force measurement.
`$THRUST,Mass,Force,Timestamp*CS`

| Field | Description | Unit | Example |
|---|---|---|---|
| Mass | Raw mass reading | g | `500.2` |
| Force | Calculated force (inverted) | N | `-4.90` |
> Note: Force is calculated as `Mass_kg * 9.81`. Negative values typically indicate thrust pushing against the stand.

### 3. Airspeed
Pitot tube differential pressure.
`$AIRSPEED,IAS,DiffPressure,Timestamp*CS`

| Field | Description | Unit | Example |
|---|---|---|---|
| IAS | Indicated Airspeed | m/s | `12.5` |
| DiffPressure | Raw differential pressure | Pa | `95.4` |

### 4. Power
Electrical power consumption.
`$power,Voltage,Current,Wattage,Timestamp*CS`

| Field | Description | Unit | Example |
|---|---|---|---|
| Voltage | Battery/Source voltage | V | `12.1` |
| Current | Current draw | A | `2.5` |
| Wattage | Power (V * I) | W | `30.25` |

---

## Example Stream
```
$RPM,0,0.00,12500*5A
$THRUST,0.00,-0.00,12500*4B
$AIRSPEED,0.00,0.00,12500*6C
$power,12.05,0.01,0.12,12500*1F
```
