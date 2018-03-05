#include "tests.h"

#include <stdbool.h>

#define TEST_SUITE transformfeedback

static GLuint buf;
static GLuint obuf;
static GLuint program;
static GLuint pipeline;
static GLuint vao;

GLuint create_tf_program(const char* vert, bool separable) {
    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vert, NULL);
    glCompileShader(vertex);
    
    GLuint program = glCreateProgram();
    glAttachShader(program, vertex);
    
    glTransformFeedbackVaryings(program, 1, (const char*[]){"output_"}, GL_INTERLEAVED_ATTRIBS);
    
    glProgramParameteri(program, GL_PROGRAM_SEPARABLE, separable);
    glLinkProgram(program);
    glValidateProgram(program);
    glDetachShader(program, vertex);
    glDeleteShader(vertex);
    
    return program;
}

#define DATA(...) (float[]){__VA_ARGS__}

#define TRANSFORMFEEDBACK_SETUP(buffer_size, buffer_data, vertex) do {\
    program = create_tf_program("#version 150\n"\
        "in vec4 input_;\n"\
        "out vec4 output_;\n"\
        "void main() {\n"\
        vertex\
        "}\n", false);\
    glUseProgram(program);\
    glBindBuffer(GL_ARRAY_BUFFER, buf);\
    glBufferData(GL_ARRAY_BUFFER, buffer_size*16, buffer_data, GL_STATIC_DRAW);\
    glEnableVertexAttribArray(glGetAttribLocation(program, "input_"));\
    glVertexAttribPointer(glGetAttribLocation(program, "input_"),\
                          4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);\
    glBindBuffer(GL_ARRAY_BUFFER, obuf);\
    glBufferData(GL_ARRAY_BUFFER, buffer_size*16, NULL, GL_STATIC_READ);\
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, obuf);\
    glEnable(GL_RASTERIZER_DISCARD);\
    glBeginTransformFeedback(GL_POINTS);\
} while (0)

#define TRANSFORMFEEDBACK_SETUP_PIPELINE(buffer_size, buffer_data, vertex) do {\
    program = create_tf_program("#version 150\n"\
        "in vec4 input_;\n"\
        "out vec4 output_;\n"\
        "void main() {\n"\
        vertex\
        "}\n", true);\
    glBindProgramPipeline(pipeline);\
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, program);\
    glBindBuffer(GL_ARRAY_BUFFER, buf);\
    glBufferData(GL_ARRAY_BUFFER, buffer_size*16, buffer_data, GL_STATIC_DRAW);\
    glEnableVertexAttribArray(glGetAttribLocation(program, "input_"));\
    glVertexAttribPointer(glGetAttribLocation(program, "input_"),\
                          4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);\
    glBindBuffer(GL_ARRAY_BUFFER, obuf);\
    glBufferData(GL_ARRAY_BUFFER, buffer_size*16, NULL, GL_STATIC_READ);\
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, obuf);\
    glEnable(GL_RASTERIZER_DISCARD);\
    glBeginTransformFeedback(GL_POINTS);\
} while (0)

void transformfeedback_setup() {
    glGenBuffers(1, &buf);
    glGenBuffers(1, &obuf);
    glBindBuffer(GL_ARRAY_BUFFER, buf);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenProgramPipelines(1, &pipeline);
}

void transformfeedback_cleanup() {
    glDeleteProgramPipelines(1, &pipeline);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &obuf);
    glDeleteBuffers(1, &buf);
}

BEGIN_TEST(basic_0)
    TRANSFORMFEEDBACK_SETUP(2, DATA(1, 2, 3, 4, 5, 6, 7, 8),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 2);
    glEndTransformFeedback();
    assert_properties(GL_BUFFER, obuf, "data", 32,
        (float[]){2, 4, 6, 8, 10, 12, 14, 16}, NULL);
END_TEST(basic_0, 320)

BEGIN_TEST(basic_1)
    TRANSFORMFEEDBACK_SETUP_PIPELINE(2, DATA(1, 2, 3, 4, 5, 6, 7, 8),
                                     "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 2);
    glEndTransformFeedback();
    assert_properties(GL_BUFFER, obuf, "data", 32,
        (float[]){2, 4, 6, 8, 10, 12, 14, 16}, NULL);
END_TEST(basic_1, 320)

BEGIN_TEST(error_0)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 16, DATA(0, 0, 0, 0));
    assert_attachment("Buffer should not be modified as it is a transform feedback one while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_0, 320)

BEGIN_TEST(error_1)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, 16, DATA(0, 0, 0, 0), GL_STATIC_DRAW);
    assert_attachment("Buffer should not be modified as it is a transform feedback one while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_1, 320)

BEGIN_TEST(error_2)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glMapBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, GL_READ_ONLY);
    assert_attachment("Buffer should not be mapped as it is a transform feedback one while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_2, 320)

/*BEGIN_TEST(error_3) //TODO: Check not yet implemented
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glBindBuffer(GL_PIXEL_PACK_BUFFER, obuf);
    float pixel[4];
    glReadPixels(0, 0, 1, 1, GL_RGBA, GL_FLOAT, pixel);
    assert_attachment("Buffer should not be read as it is a transform feedback one while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_3, 320)*/

BEGIN_TEST(error_4)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glUseProgram(0);
    assert_attachment("The program binding cannot be modified while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_4, 320)

BEGIN_TEST(error_5)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glLinkProgram(program);
    assert_attachment("The bound program cannot be modified while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_5, 320)

BEGIN_TEST(error_6)
    TRANSFORMFEEDBACK_SETUP(1, DATA(1, 2, 3, 4),
                            "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glBindProgramPipeline(0);
    assert_attachment("The program pipeline binding cannot be modified while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_6, 320)

BEGIN_TEST(error_7)
    TRANSFORMFEEDBACK_SETUP_PIPELINE(1, DATA(1, 2, 3, 4),
                                     "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glUseProgramStages(pipeline, GL_VERTEX_SHADER_BIT, 0);
    assert_attachment("The bound program pipeline object cannot be modified while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_7, 320)

/*BEGIN_TEST(error_8) //TODO: Should this raise an error?
//Mesa doesn't seem to
    TRANSFORMFEEDBACK_SETUP_PIPELINE(1, DATA(1, 2, 3, 4),
                                     "output_ = input_ * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glLinkProgram(program);
    assert_attachment("A used program cannot be modified while transform feedback is active and unpaused");
    glEndTransformFeedback();
END_TEST(error_8, 320)*/

BEGIN_TEST(error_9)
    TRANSFORMFEEDBACK_SETUP_PIPELINE(1, DATA(1, 2, 3, 4),
                                     "output_ = input * 2.0;");
    glDrawArrays(GL_POINTS, 0, 1);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0);
    assert_attachment("Cannot modify GL_TRANSFORM_FEEDBACK_BUFFER when transform feedback is active and not paused");
    glEndTransformFeedback();
END_TEST(error_9, 320)

BEGIN_TEST(transform_feedback_object_0)
//does not work because of a Nvidia driver bug
//https://stackoverflow.com/questions/39501309/buffer-bindings-of-transform-feedback-objects
    /*TRANSFORMFEEDBACK_SETUP(2, DATA(1, 2, 3, 4, 5, 6, 7, 8),
                            "output_ = input * 2.0;");
    glEndTransformFeedback();
    
    GLuint obuf2;
    glGenBuffers(1, &obuf2);
    glBindBuffer(GL_ARRAY_BUFFER, obuf2);
    glBufferData(GL_ARRAY_BUFFER, 32, NULL, GL_STATIC_READ);
    
    GLuint tfo;
    glGenTransformFeedbacks(1, &tfo);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfo);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, obuf2);
    
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 2);
    glEndTransformFeedback();
    //float data[8];
    //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 32, data);
    //printf("%g %g %g %g %g %g %g %g\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    
    glBindBuffer(GL_ARRAY_BUFFER, obuf);
    glVertexAttribPointer(glGetAttribLocation(program, "input"),
                          4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
    glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, tfo);
    glBeginTransformFeedback(GL_POINTS);
    glDrawArrays(GL_POINTS, 0, 2);
    glEndTransformFeedback();
    //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 32, data);
    //printf("%g %g %g %g %g %g %g %g\n", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
    
    assert_properties(GL_BUFFER, obuf, "data", 32,
        (float[]){2, 4, 6, 8, 10, 12, 14, 16}, NULL);
    assert_properties(GL_BUFFER, obuf2, "data", 32,
        (float[]){4, 8, 12, 16, 20, 24, 28, 32}, NULL);
    
    glDeleteTransformFeedbacks(1, &tfo);
    glDeleteBuffers(1, &obuf2);*/
END_TEST(transform_feedback_object_0, 320)

//TODO: More transform feedback object tests
