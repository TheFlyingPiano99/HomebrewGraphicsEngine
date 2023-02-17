
# Shaders

These are the most important vertex and fragment shader source file names provided for Hogra engine.

The naming convention for vertex shaders is the following:
(single / instanced)(2D / 3D)(- / WithTBN).vert

The naming convention for fragment shaders is the following:
(forward / deferred)(Homogene / Mapped / Emissive / ...).frag


## Vertex shaders

- single2D.vert
- single3D.vert
- single3DWithTBN.vert
- instanced2D.vert
- instanced3D.vert
- instanced3DWithTBN.vert


## Fragment shaders

- deferredHomogeneGeometryPass.frag
- deferredMappedGeometryPass.frag
- deferredPBRDirectionalLightingPass.frag
- deferredPBRLightVolumePass.frag
- deferredEmissiveGeometryPass.frag
