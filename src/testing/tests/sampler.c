#include "tests.h"
#include <stdarg.h>

#define TEST_SUITE sampler

static GLuint sampler;

static void param(int mode, GLenum pname, size_t count, ...) {
    va_list list;
    va_start(list, count);
    
    GLfloat valf[16];
    GLint vali[16];
    GLuint valu[16];
    for (size_t i = 0; i < count; i++) {
        double v = va_arg(list, double);
        valf[i] = v;
        vali[i] = v;
        valu[i] = v;
    }
    
    switch (mode) {
    case 0: glSamplerParameterf(sampler, pname, valf[0]); break;
    case 1: glSamplerParameteri(sampler, pname, vali[0]); break;
    case 2: glSamplerParameterfv(sampler, pname, valf); break;
    case 3: glSamplerParameteriv(sampler, pname, vali); break;
    case 4: glSamplerParameterIiv(sampler, pname, vali); break;
    case 5: glSamplerParameterIuiv(sampler, pname, valu); break;
    }
    
    va_end(list);
}

void sampler_setup() {
    glGenSamplers(1, &sampler);
}

void sampler_cleanup() {
    glDeleteSamplers(1, &sampler);
}

BEGIN_TEST(default_properties)
    assert_properties(GL_SAMPLER, sampler, "*", NULL);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 0, 0.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 1, 0.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 2, 0.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 3, 0.0);
END_TEST(default_properties, 320)

BEGIN_TEST(glSamplerParameter_0)
    glSamplerParameteri(sampler, GL_TEXTURE_MIN_FILTER, 13);
    assert_attachment("Invalid minification filter");
    
    glSamplerParameteri(sampler, GL_TEXTURE_MAG_FILTER, 11);
    assert_attachment("Invalid magnification filter");
    
    glSamplerParameterf(sampler, GL_TEXTURE_BORDER_COLOR, 1.0);
    assert_attachment("Expected 4 values. Got 1.");
    
    glSamplerParameteri(sampler, GL_TEXTURE_WRAP_S, 62);
    assert_attachment("Invalid wrap mode");
    
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_MODE, 41);
    assert_attachment("Invalid compare mode");
    
    glSamplerParameteri(sampler, GL_TEXTURE_COMPARE_FUNC, 77);
    assert_attachment("Invalid compare function");
    
    //TODO: Have tests explicitly enable extensions so that this works
    /*glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 1.0);
    assert_attachment("pname not supported with current version and extensions");*/
    
    //TODO: With the version < 4.4: check that GL_MIRROR_CLAMP_TO_EDGE fails
END_TEST(glSamplerParameter_0, 320)

BEGIN_TEST(glSamplerParameter_0_maxaniso)
    glSamplerParameterf(sampler, GL_TEXTURE_MAX_ANISOTROPY, 0.1);
    assert_attachment("Invalid max anisotropy");
END_TEST(glSamplerParameter_0_maxaniso, 460)

BEGIN_TEST(glSamplerParameter_1)
    for (int i = 0; i < 6; i++) {
        param(i, GL_TEXTURE_MIN_FILTER, 1, (double)GL_LINEAR);
        assert_properties(GL_SAMPLER, sampler, "min_filter", GL_LINEAR, NULL);
        
        param(i, GL_TEXTURE_MAG_FILTER, 1, (double)GL_NEAREST);
        assert_properties(GL_SAMPLER, sampler, "mag_filter", GL_NEAREST, NULL);
        
        param(i, GL_TEXTURE_MIN_LOD, 1, 500.0);
        assert_properties(GL_SAMPLER, sampler, "min_lod", 500, NULL);
        
        param(i, GL_TEXTURE_MAX_LOD, 1, 700.0);
        assert_properties(GL_SAMPLER, sampler, "max_lod", 700, NULL);
        
        param(i, GL_TEXTURE_WRAP_S, 1, (double)GL_CLAMP_TO_BORDER);
        assert_properties(GL_SAMPLER, sampler, "wrap_s", GL_CLAMP_TO_BORDER, NULL);
        
        param(i, GL_TEXTURE_WRAP_T, 1, (double)GL_MIRRORED_REPEAT);
        assert_properties(GL_SAMPLER, sampler, "wrap_t", GL_MIRRORED_REPEAT, NULL);
        
        param(i, GL_TEXTURE_WRAP_R, 1, (double)GL_CLAMP_TO_EDGE);
        assert_properties(GL_SAMPLER, sampler, "wrap_r", GL_CLAMP_TO_EDGE, NULL);
        
        param(i, GL_TEXTURE_LOD_BIAS, 1, 2.0);
        assert_properties(GL_SAMPLER, sampler, "lod_bias", 2.0, NULL);
        
        //TODO: max anisotropy
    }
    
    param(2, GL_TEXTURE_BORDER_COLOR, 4, 5.0, 4.0, 3.0, 2.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 0, 5.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 1, 4.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 2, 3.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 3, 2.0);
    
    param(3, GL_TEXTURE_BORDER_COLOR, 4, 2147483647.0, 1073741824.0, 536870912.0, 268435456.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 0, 1.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 1, 0.5);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 2, 0.25);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 3, 0.125);
    
    param(4, GL_TEXTURE_BORDER_COLOR, 4, 55.0, 44.0, 33.0, 22.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 0, 55.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 1, 44.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 2, 33.0);
    wip15ExpectPropertyd(GL_SAMPLER, sampler, "border_color", 3, 22.0);
END_TEST(glSamplerParameter_1, 320)

