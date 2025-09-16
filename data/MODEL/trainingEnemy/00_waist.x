xof 0302txt 0064
template Header {
 <3D82AB43-62DA-11cf-AB39-0020AF71E433>
 WORD major;
 WORD minor;
 DWORD flags;
}

template Vector {
 <3D82AB5E-62DA-11cf-AB39-0020AF71E433>
 FLOAT x;
 FLOAT y;
 FLOAT z;
}

template Coords2d {
 <F6F23F44-7686-11cf-8F52-0040333594A3>
 FLOAT u;
 FLOAT v;
}

template Matrix4x4 {
 <F6F23F45-7686-11cf-8F52-0040333594A3>
 array FLOAT matrix[16];
}

template ColorRGBA {
 <35FF44E0-6C7C-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
 FLOAT alpha;
}

template ColorRGB {
 <D3E16E81-7835-11cf-8F52-0040333594A3>
 FLOAT red;
 FLOAT green;
 FLOAT blue;
}

template IndexedColor {
 <1630B820-7842-11cf-8F52-0040333594A3>
 DWORD index;
 ColorRGBA indexColor;
}

template Boolean {
 <4885AE61-78E8-11cf-8F52-0040333594A3>
 WORD truefalse;
}

template Boolean2d {
 <4885AE63-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template MaterialWrap {
 <4885AE60-78E8-11cf-8F52-0040333594A3>
 Boolean u;
 Boolean v;
}

template TextureFilename {
 <A42790E1-7810-11cf-8F52-0040333594A3>
 STRING filename;
}

template Material {
 <3D82AB4D-62DA-11cf-AB39-0020AF71E433>
 ColorRGBA faceColor;
 FLOAT power;
 ColorRGB specularColor;
 ColorRGB emissiveColor;
 [...]
}

template MeshFace {
 <3D82AB5F-62DA-11cf-AB39-0020AF71E433>
 DWORD nFaceVertexIndices;
 array DWORD faceVertexIndices[nFaceVertexIndices];
}

template MeshFaceWraps {
 <4885AE62-78E8-11cf-8F52-0040333594A3>
 DWORD nFaceWrapValues;
 Boolean2d faceWrapValues;
}

template MeshTextureCoords {
 <F6F23F40-7686-11cf-8F52-0040333594A3>
 DWORD nTextureCoords;
 array Coords2d textureCoords[nTextureCoords];
}

template MeshMaterialList {
 <F6F23F42-7686-11cf-8F52-0040333594A3>
 DWORD nMaterials;
 DWORD nFaceIndexes;
 array DWORD faceIndexes[nFaceIndexes];
 [Material]
}

template MeshNormals {
 <F6F23F43-7686-11cf-8F52-0040333594A3>
 DWORD nNormals;
 array Vector normals[nNormals];
 DWORD nFaceNormals;
 array MeshFace faceNormals[nFaceNormals];
}

template MeshVertexColors {
 <1630B821-7842-11cf-8F52-0040333594A3>
 DWORD nVertexColors;
 array IndexedColor vertexColors[nVertexColors];
}

template Mesh {
 <3D82AB44-62DA-11cf-AB39-0020AF71E433>
 DWORD nVertices;
 array Vector vertices[nVertices];
 DWORD nFaces;
 array MeshFace faces[nFaces];
 [...]
}

Header{
1;
0;
1;
}

Mesh {
 28;
 28.67335;0.03752;-13.53188;,
 28.67335;0.03752;13.13174;,
 28.67335;-33.16383;13.13174;,
 28.67335;-33.16383;-13.53188;,
 -27.67200;0.03752;13.66994;,
 -27.67200;0.03752;-13.53188;,
 -27.67200;-33.16383;-13.53188;,
 -27.67200;-33.16383;13.66994;,
 -0.66289;0.03747;-29.04956;,
 28.67335;0.03752;-13.53188;,
 28.67335;-33.16383;-13.53188;,
 -0.49489;-33.16383;-29.04956;,
 -27.67200;-33.16383;-13.53188;,
 -27.67200;0.03752;-13.53188;,
 0.58621;0.03752;25.07585;,
 -27.67200;0.03752;13.66994;,
 -27.67200;-33.16383;13.66994;,
 0.75421;-33.16383;19.21985;,
 28.67335;-33.16383;13.13174;,
 28.67335;0.03752;13.13174;,
 0.58621;0.03752;25.07585;,
 28.67335;0.03752;-13.53188;,
 -0.66289;0.03747;-29.04956;,
 -27.67200;0.03752;-13.53188;,
 -0.49489;-33.16383;-29.04956;,
 28.67335;-33.16383;-13.53188;,
 0.75421;-33.16383;19.21985;,
 -27.67200;-33.16383;-13.53188;;
 
 10;
 4;0,1,2,3;,
 4;4,5,6,7;,
 4;8,9,10,11;,
 4;8,11,12,13;,
 4;14,15,16,17;,
 4;14,17,18,19;,
 4;20,1,21,22;,
 4;20,22,23,4;,
 4;24,25,2,26;,
 4;24,26,7,27;;
 
 MeshMaterialList {
  3;
  10;
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1,
  1;;
  Material {
   0.000000;0.000000;0.000000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   0.706000;0.722000;0.733000;1.000000;;
   5.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
  Material {
   1.000000;1.000000;1.000000;1.000000;;
   0.000000;
   0.000000;0.000000;0.000000;;
   0.000000;0.000000;0.000000;;
  }
 }
 MeshNormals {
  12;
  -1.000000;0.000000;0.000000;,
  1.000000;0.000000;0.000000;,
  -0.016206;-0.000041;-0.999869;,
  0.010104;-0.084709;0.996355;,
  0.468625;0.001183;-0.883396;,
  -0.497008;-0.001255;-0.867745;,
  -0.282669;-0.082263;0.955684;,
  0.301994;-0.079753;0.949968;,
  0.000000;1.000000;-0.000000;,
  -0.000001;1.000000;-0.000000;,
  0.000001;1.000000;-0.000000;,
  0.000000;-1.000000;-0.000000;;
  10;
  4;1,1,1,1;,
  4;0,0,0,0;,
  4;2,4,4,2;,
  4;2,2,5,5;,
  4;3,6,6,3;,
  4;3,3,7,7;,
  4;8,9,9,8;,
  4;8,8,10,10;,
  4;11,11,11,11;,
  4;11,11,11,11;;
 }
 MeshTextureCoords {
  28;
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.000000;1.000000;,
  0.483870;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.486950;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.510080;0.000000;,
  1.000000;0.000000;,
  1.000000;1.000000;,
  0.506990;1.000000;,
  0.000000;1.000000;,
  0.000000;0.000000;,
  0.489920;0.000000;,
  1.000000;1.000000;,
  0.483870;1.000000;,
  0.000000;1.000000;,
  0.486950;0.000000;,
  1.000000;0.000000;,
  0.493010;1.000000;,
  0.000000;0.000000;;
 }
}
