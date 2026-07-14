# Star Catcher

A tiny original Game Boy arcade game built with GBDK. Pilot the ship, catch stars,
and avoid meteors as the falling objects speed up.

## Controls

- **Left / Right:** Move the ship
- **Start:** Pause or resume
- **Start / A:** Begin the game

## Build

```sh
make
```

This uses the GBDK installation at `../gbdk` and creates `star-catcher.gb`.
Override the path if needed with `make GBDK_HOME=/path/to/gbdk`.

To build the AOS and SOA object-update benchmark ROMs:

```sh
make benchmark-layout
```

Run `benchmark-aos.gb` and `benchmark-soa.gb` in a Game Boy emulator. Each ROM
shows the elapsed hardware-divider ticks and a checksum; matching checksums
confirm that both layouts performed the same work. Lower tick counts are better.

To launch through an emulator command available on your system:

```sh
make run EMULATOR=binjgb
```

The graphics are hand-drawn 2bpp Game Boy tiles in `src/main.c`. Sound uses the
Game Boy's pulse and noise channels directly, so no generated assets or runtime
dependencies are required.
