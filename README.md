# SpaceGame

## Important Notes

### Blender to SDKMESH

Use the following command when converting Blender-exported meshes to SDKMESH format:

```bash
meshconvert <MODEL>.obj -o <MODEL>.SDKMESH -sdkmesh2 -flipv -tb
```

- `-sdkmesh` : Convert to SDKMESH version 2 which supports PBR pipeline
- `-flipv` : Flips the V coordinate which is backwards in DirectX by default
- `-tb` : Generates tangents and bitangents

-----

### IBL Map Exporting

Use the following DDS export settings when creating radiance/irradiance lightmaps:

- **Format**: BC7
- **Texture Type**: Cubemap
- **Generate Mips**: Yes
- **Compression**: Production

The irradiance map can be generated one of two ways:

1. By applying convolution to the radiance map.
2. By taking the first mip level of the radiance map and upscaling it to 128x128.

-----

### Formats

The 10:10:10:2 format Microsoft uses in its examples for HDR scenes is unsupported by Direct2D. This project
instead uses an 8:8:8:8 format which results in slightly less accurate color information
but allows me to draw the UI/HUD elements much easier.