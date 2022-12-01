# Projects 5 & 6: Lights, Camera & Action!

All project handouts can be found [here](https://cs1230.graphics/projects).


# Project 5:
Bugs:
- No known bugs


Design:
- Copied the code used from labs 5,8,10. Used a shape class for each of the primitives which had the functions necessary to generate their vertex and normal data.
- Kept track of scene data by saving into RenderData m_metaData variable, like in raytrace.
- Used an array of 8 light structs to compute lighting in the vertex shader.
- Added an inverse transpose field to RenderShapeData, so the calculation only happens once while parsing the scene data. 
- Camera class with helper functions for producing new projection and view matrices based on setting changes
- Initialize all four vbos and vaos in initializeGL()


# Project 6:
Bugs:
    - No known bugs

Extra Credit:
    - Extra Credit 1: Grayscale filter
    - Extra Credit 2: Sharpen filter
    