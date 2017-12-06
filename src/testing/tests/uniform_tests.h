//============================== Float Uniforms ==============================

BEGIN_UNIFORM_TEST(float_0)
    UNIFORM_SETUP("uniform float u;\n", "vec4(u)");
    U(1f, "u", 1.f);
    uniform_testd("u", 1, 1.);
END_UNIFORM_TEST(float_0)

BEGIN_UNIFORM_TEST(vec2_0)
    UNIFORM_SETUP("uniform vec2 u;\n", "vec4(u, u)");
    U(2f, "u", 1.f, 2.f);
    uniform_testd("u", 2, 1., 2.);
END_UNIFORM_TEST(vec2_0)

BEGIN_UNIFORM_TEST(vec3_0)
    UNIFORM_SETUP("uniform vec3 u;\n", "vec4(u, 1.)");
    U(3f, "u", 1.f, 2.f, 3.f);
    uniform_testd("u", 3, 1., 2., 3.);
END_UNIFORM_TEST(vec3_0)

BEGIN_UNIFORM_TEST(vec4_0)
    UNIFORM_SETUP("uniform vec4 u;\n", "u");
    U(4f, "u", 1.f, 2.f, 3.f, 4.f);
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_UNIFORM_TEST(vec4_0)

BEGIN_UNIFORM_TEST(float_vec2_0)
    UNIFORM_SETUP("uniform float u0;\nuniform vec2 u1;\n", "vec4(u0, u0, u1)");
    U(1f, "u0", 1.f);
    U(2f, "u1", 2.f, 3.f);
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_UNIFORM_TEST(float_vec2_0)

//============================= Integer Uniforms =============================

BEGIN_UNIFORM_TEST(int_0)
    UNIFORM_SETUP("uniform int u;\n", "vec4(u)");
    U(1i, "u", 1);
    uniform_testd("u", 1, 1.);
END_UNIFORM_TEST(int_0)

BEGIN_UNIFORM_TEST(ivec2_0)
    UNIFORM_SETUP("uniform ivec2 u;\n", "vec4(u, u)");
    U(2i, "u", 1, 2);
    uniform_testd("u", 2, 1., 2.);
END_UNIFORM_TEST(ivec2_0)

BEGIN_UNIFORM_TEST(ivec3_0)
    UNIFORM_SETUP("uniform ivec3 u;\n", "vec4(u, 1.)");
    U(3i, "u", 1, 2, 3);
    uniform_testd("u", 3, 1., 2., 3.);
END_UNIFORM_TEST(ivec3_0)

BEGIN_UNIFORM_TEST(ivec4_0)
    UNIFORM_SETUP("uniform ivec4 u;\n", "u");
    U(4i, "u", 1, 2, 3, 4);
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_UNIFORM_TEST(ivec4_0)

BEGIN_UNIFORM_TEST(int_ivec2_0)
    UNIFORM_SETUP("uniform int u0;\nuniform ivec2 u1;\n", "vec4(u0, u0, u1)");
    U(1i, "u0", 1);
    U(2i, "u1", 2, 3);
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_UNIFORM_TEST(int_ivec2_0)

//========================= Unsigned Integer Uniforms =========================

BEGIN_UNIFORM_TEST(uint_0)
    UNIFORM_SETUP("uniform uint u;\n", "vec4(u)");
    U(1ui, "u", 1);
    uniform_testd("u", 1, 1.);
END_UNIFORM_TEST(uint_0)

BEGIN_UNIFORM_TEST(uvec2_0)
    UNIFORM_SETUP("uniform uvec2 u;\n", "vec4(u, u)");
    U(2ui, "u", 1, 2);
    uniform_testd("u", 2, 1., 2.);
END_UNIFORM_TEST(uvec2_0)

BEGIN_UNIFORM_TEST(uvec3_0)
    UNIFORM_SETUP("uniform uvec3 u;\n", "vec4(u, 1.)");
    U(3ui, "u", 1, 2, 3);
    uniform_testd("u", 3, 1., 2., 3.);
END_UNIFORM_TEST(uvec3_0)

BEGIN_UNIFORM_TEST(uvec4_0)
    UNIFORM_SETUP("uniform uvec4 u;\n", "u");
    U(4ui, "u", 1, 2, 3, 4);
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_UNIFORM_TEST(uvec4_0)

BEGIN_UNIFORM_TEST(uint_uvec2_0)
    UNIFORM_SETUP("uniform uint u0;\nuniform uvec2 u1;\n", "vec4(u0, u0, u1)");
    U(1ui, "u0", 1);
    U(2ui, "u1", 2, 3);
    uniform_testd("u0", 1, 1.);
    uniform_testd("u1", 2, 2., 3.);
END_UNIFORM_TEST(uint_uvec2_0)

//=========================== Float Array Uniforms ===========================

BEGIN_UNIFORM_TEST(float_arr_0)
    UNIFORM_SETUP("uniform float u[2];\n", "vec4(u[0], u[1], 0., 0.)");
    U(1fv, "u", 2, (float[]){1.f, 2.f});
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_UNIFORM_TEST(float_arr_0)

BEGIN_UNIFORM_TEST(vec2_arr_0)
    UNIFORM_SETUP("uniform vec2 u[2];\n", "vec4(u[0], u[1])");
    U(2fv, "u", 2, (float[]){1.f, 2.f, 3.f, 4.f});
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_UNIFORM_TEST(vec2_arr_0)

BEGIN_UNIFORM_TEST(vec3_arr_0)
    UNIFORM_SETUP("uniform vec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(3fv, "u", 2, (float[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f});
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_UNIFORM_TEST(vec3_arr_0)

BEGIN_UNIFORM_TEST(vec4_arr_0)
    UNIFORM_SETUP("uniform vec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(4fv, "u", 2, (float[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f});
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_UNIFORM_TEST(vec4_arr_0)

//========================== Integer Array Uniforms ==========================

BEGIN_UNIFORM_TEST(int_arr_0)
    UNIFORM_SETUP("uniform int u[2];\n", "vec4(u[0], u[1], 0, 0)");
    U(1iv, "u", 2, (GLint[]){1, 2});
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_UNIFORM_TEST(int_arr_0)

BEGIN_UNIFORM_TEST(ivec2_arr_0)
    UNIFORM_SETUP("uniform ivec2 u[2];\n", "vec4(u[0], u[1])");
    U(2iv, "u", 2, (GLint[]){1, 2, 3, 4});
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_UNIFORM_TEST(ivec2_arr_0)

BEGIN_UNIFORM_TEST(ivec3_arr_0)
    UNIFORM_SETUP("uniform ivec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(3iv, "u", 2, (GLint[]){1, 2, 3, 4, 5, 6});
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_UNIFORM_TEST(ivec3_arr_0)

BEGIN_UNIFORM_TEST(ivec4_arr_0)
    UNIFORM_SETUP("uniform ivec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(4iv, "u", 2, (GLint[]){1, 2, 3, 4, 5, 6, 7, 8});
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_UNIFORM_TEST(ivec4_arr_0)

//====================== Unsigned Integer Array Uniforms ======================

BEGIN_UNIFORM_TEST(uint_arr_0)
    UNIFORM_SETUP("uniform uint u[2];\n", "vec4(u[0], u[1], 0, 0)");
    U(1uiv, "u", 2, (GLuint[]){1, 2});
    uniform_testd("u[0]", 1, 1.);
    uniform_testd("u[1]", 1, 2.);
END_UNIFORM_TEST(uint_arr_0)

BEGIN_UNIFORM_TEST(uvec2_arr_0)
    UNIFORM_SETUP("uniform uvec2 u[2];\n", "vec4(u[0], u[1])");
    U(2uiv, "u", 2, (GLuint[]){1, 2, 3, 4});
    uniform_testd("u[0]", 2, 1., 2.);
    uniform_testd("u[1]", 2, 3., 4.);
END_UNIFORM_TEST(uvec2_arr_0)

BEGIN_UNIFORM_TEST(uvec3_arr_0)
    UNIFORM_SETUP("uniform uvec3 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(3uiv, "u", 2, (GLuint[]){1, 2, 3, 4, 5, 6});
    uniform_testd("u[0]", 3, 1., 2., 3.);
    uniform_testd("u[1]", 3, 4., 5., 6.);
END_UNIFORM_TEST(uvec3_arr_0)

BEGIN_UNIFORM_TEST(uvec4_arr_0)
    UNIFORM_SETUP("uniform uvec4 u[2];\n", "vec4(u[0].xy, u[1].xy)");
    U(4uiv, "u", 2, (GLuint[]){1, 2, 3, 4, 5, 6, 7, 8});
    uniform_testd("u[0]", 4, 1., 2., 3., 4.);
    uniform_testd("u[1]", 4, 5., 6., 7., 8.);
END_UNIFORM_TEST(uvec4_arr_0)

//============================== Matrix Uniforms ==============================

BEGIN_UNIFORM_TEST(mat2_0)
    UNIFORM_SETUP("uniform mat2 u;\n", "vec4(u[0], u[1])");
    U(Matrix2fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f});
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_UNIFORM_TEST(mat2_0)

BEGIN_UNIFORM_TEST(mat2_1)
    UNIFORM_SETUP("uniform mat2 u;\n", "vec4(u[0], u[1])");
    U(Matrix2fv, "u", 1, GL_TRUE, (GLfloat[]){1.f, 3.f, 2.f, 4.f});
    uniform_testd("u", 4, 1., 2., 3., 4.);
END_UNIFORM_TEST(mat2_1)

BEGIN_UNIFORM_TEST(mat3_0)
    UNIFORM_SETUP("uniform mat3 u;\n", "vec4(u[0], u[1].x)");
    U(Matrix3fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f});
    uniform_testd("u", 9, 1., 2., 3., 4., 5., 6., 7., 8., 9.);
END_UNIFORM_TEST(mat3_0)

BEGIN_UNIFORM_TEST(mat3_1)
    UNIFORM_SETUP("uniform mat3 u;\n", "vec4(u[0], u[1].x)");
    U(Matrix3fv, "u", 1, GL_TRUE, (GLfloat[]){1.f, 4.f, 7.f, 2.f, 5.f, 8.f, 3.f, 6.f, 9.f});
    uniform_testd("u", 9, 1., 2., 3., 4., 5., 6., 7., 8., 9.);
END_UNIFORM_TEST(mat3_1)

BEGIN_UNIFORM_TEST(mat4_0)
    UNIFORM_SETUP("uniform mat4 u;\n", "u[0]");
    U(Matrix4fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f, 13.f, 14.f, 15.f, 16.f});
    uniform_testd("u", 16, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 16.);
END_UNIFORM_TEST(mat4_0)

BEGIN_UNIFORM_TEST(mat4_1)
    UNIFORM_SETUP("uniform mat4 u;\n", "u[0]");
    U(Matrix4fv, "u", 1, GL_TRUE, (GLfloat[]){1.f, 5.f, 9.f, 13.f, 2.f, 6.f, 10.f, 14.f, 3.f, 7.f, 11.f, 15.f, 4.f, 8.f, 12.f, 16.f});
    uniform_testd("u", 16, 1., 2., 3., 4., 5., 6., 7., 8., 9., 10., 11., 12., 13., 14., 15., 16.);
END_UNIFORM_TEST(mat4_1)

BEGIN_UNIFORM_TEST(mat2x3_0)
    UNIFORM_SETUP("uniform mat2x3 u;\n", "vec4(u[0], u[1].x)");
    U(Matrix2x3fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f});
    uniform_testd("u", 6, 1., 2., 3., 4., 5., 6.);
END_UNIFORM_TEST(mat2x3_0)

BEGIN_UNIFORM_TEST(mat2x3_1)
    UNIFORM_SETUP("uniform mat2x3 u;\n", "vec4(u[0], u[1].x)");
    U(Matrix2x3fv, "u", 1, GL_TRUE, (GLfloat[]){1.f, 4.f, 2.f, 5.f, 3.f, 6.f});
    uniform_testd("u", 6, 1., 2., 3., 4., 5., 6.);
END_UNIFORM_TEST(mat2x3_1)

BEGIN_UNIFORM_TEST(mat3x2_0)
    UNIFORM_SETUP("uniform mat3x2 u;\n", "vec4(u[0], u[1])");
    U(Matrix3x2fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f});
    uniform_testd("u", 6, 1., 2., 3., 4., 5., 6.);
END_UNIFORM_TEST(mat3x2_0)

BEGIN_UNIFORM_TEST(mat2x4_0)
    UNIFORM_SETUP("uniform mat2x4 u;\n", "u[0]");
    U(Matrix2x4fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f});
    uniform_testd("u", 8, 1., 2., 3., 4., 5., 6., 7., 8.);
END_UNIFORM_TEST(mat2x4_0)

BEGIN_UNIFORM_TEST(mat4x2_0)
    UNIFORM_SETUP("uniform mat4x2 u;\n", "vec4(u[0], u[1])");
    U(Matrix4x2fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f});
    uniform_testd("u", 8, 1., 2., 3., 4., 5., 6., 7., 8.);
END_UNIFORM_TEST(mat4x2_0)

BEGIN_UNIFORM_TEST(mat3x4_0)
    UNIFORM_SETUP("uniform mat3x4 u;\n", "u[0]");
    U(Matrix3x4fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f});
    uniform_testd("u", 12, 1., 2., 3., 4., 5., 6., 7., 8., 9.f, 10.f, 11.f, 12.f);
END_UNIFORM_TEST(mat3x4_0)

BEGIN_UNIFORM_TEST(mat4x3_0)
    UNIFORM_SETUP("uniform mat4x3 u;\n", "vec4(u[0], u[1].x)");
    U(Matrix4x3fv, "u", 1, GL_FALSE, (GLfloat[]){1.f, 2.f, 3.f, 4.f, 5.f, 6.f, 7.f, 8.f, 9.f, 10.f, 11.f, 12.f});
    uniform_testd("u", 12, 1., 2., 3., 4., 5., 6., 7., 8., 9.f, 10.f, 11.f, 12.f);
END_UNIFORM_TEST(mat4x3_0)
