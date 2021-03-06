# Create a custom command that runs the Python script
# ${generator_script} to generate tests.
#
# A list of the files generated will be output to the file
# ${file_list}.  This can be used to run the custom command by itself,
# and is also used by the build system to tell when the files need to
# be rebuilt.
#
# The custom command will automatically depend on ${generator_script}.
# Additional dependencies can be supplied using additional arguments.
function(piglit_make_generated_tests file_list generator_script)
	# Add a custom command which executes ${generator_script}
	# during the build.
	add_custom_command(
		OUTPUT ${file_list}
		COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${generator_script} > ${file_list}
		DEPENDS ${generator_script} ${ARGN}
		VERBATIM)
endfunction(piglit_make_generated_tests custom_target generator_script)

# Generators for OpenGL tests
piglit_make_generated_tests(
	builtin_packing_tests.list
	gen_builtin_packing_tests.py
	templates/gen_builtin_packing_tests/vs_unpack.shader_test.mako
	templates/gen_builtin_packing_tests/vs_pack.shader_test.mako
	templates/gen_builtin_packing_tests/fs_unpack.shader_test.mako
	templates/gen_builtin_packing_tests/fs_pack.shader_test.mako
	templates/gen_builtin_packing_tests/const_pack.shader_test.mako
	templates/gen_builtin_packing_tests/const_unpack.shader_test.mako
	)
piglit_make_generated_tests(
	builtin_uniform_tests.list
	gen_builtin_uniform_tests.py
	builtin_function.py)
piglit_make_generated_tests(
	constant_array_size_tests.list
	gen_constant_array_size_tests.py
	builtin_function.py)
piglit_make_generated_tests(
	const_builtin_equal_tests.list
	gen_const_builtin_equal_tests.py
	templates/gen_const_builtin_equal_tests/template.shader_test.mako
	)
piglit_make_generated_tests(
	interpolation_tests.list
	gen_interpolation_tests.py
	templates/gen_interpolation_tests/template.shader_test.mako
	)
piglit_make_generated_tests(
	non-lvalue_tests.list
	gen_non-lvalue_tests.py
	templates/gen_non-lvalue_tests/assignment.glsl_parser_test.mako
	templates/gen_non-lvalue_tests/out-parameter.glsl_parser_test.mako
	)
piglit_make_generated_tests(
	texture_query_lod_tests.list
	gen_texture_query_lod_tests.py
	templates/gen_texture_query_lod_tests/template.glsl_parser_test.mako
	)
piglit_make_generated_tests(
	shader_bit_encoding_tests.list
	gen_shader_bit_encoding_tests.py
	templates/gen_shader_bit_encoding_tests/template.shader_test.mako
	)
piglit_make_generated_tests(
	uniform-initializer_tests.list
	gen_uniform_initializer_tests.py
	templates/gen_uniform_initializer_tests/fs-initializer.shader_test.mako
	templates/gen_uniform_initializer_tests/vs-initializer.shader_test.mako
	templates/gen_uniform_initializer_tests/fs-initializer-from-const.shader_test.mako
	templates/gen_uniform_initializer_tests/vs-initializer-from-const.shader_test.mako
	templates/gen_uniform_initializer_tests/fs-initializer-set-by-API.shader_test.mako
	templates/gen_uniform_initializer_tests/vs-initializer-set-by-API.shader_test.mako
	templates/gen_uniform_initializer_tests/fs-initializer-set-by-other-stage.shader_test.mako
	templates/gen_uniform_initializer_tests/vs-initializer-set-by-other-stage.shader_test.mako
	)
piglit_make_generated_tests(
	interpolation-qualifier-built-in-variable.list
	interpolation-qualifier-built-in-variable.py
	templates/interpolation-qualifier-built-in-variable/fs-vs-unused.shader_test.mako
	templates/interpolation-qualifier-built-in-variable/vs-unused.shader_test.mako
	templates/interpolation-qualifier-built-in-variable/fs-unused.shader_test.mako
	templates/interpolation-qualifier-built-in-variable/vs-fs-flip.shader_test.mako
	templates/interpolation-qualifier-built-in-variable/vs-fs.shader_test.mako
	)
piglit_make_generated_tests(
	texture_lod_tests.list
	gen_texture_lod_tests.py
	templates/gen_texture_lod_tests/tex_grad.vert.mako
	templates/gen_texture_lod_tests/tex_grad.frag.mako
	templates/gen_texture_lod_tests/frag_lod.glsl_parser_test.mako
	)
piglit_make_generated_tests(
	outerproduct_invalid_params.list
	gen_outerproduct_invalid_params.py
	templates/gen_outerproduct_invalid_params/template.vert.mako
	)
piglit_make_generated_tests(
	outerproduct_tests.list
	gen_outerproduct_tests.py
	templates/gen_outerproduct_tests/template.shader_test.mako
	)

piglit_make_generated_tests(
	builtin_uniform_tests_fp64.list
	gen_builtin_uniform_tests_fp64.py
	builtin_function_fp64.py)
piglit_make_generated_tests(
	constant_array_size_tests_fp64.list
	gen_constant_array_size_tests_fp64.py
	builtin_function_fp64.py)
piglit_make_generated_tests(
	shader_precision_tests.list
	gen_shader_precision_tests.py
	builtin_function.py
	templates/gen_shader_precision_tests/vs.mako
	templates/gen_shader_precision_tests/fs.mako
	templates/gen_shader_precision_tests/gs.mako)
piglit_make_generated_tests(
	shader_image_load_store_tests.list
	gen_shader_image_load_store_tests.py)
piglit_make_generated_tests(
	variable_index_read_tests.list
	gen_variable_index_read_tests.py
	templates/gen_variable_index_read_tests/vs.shader_test.mako
	templates/gen_variable_index_read_tests/fs.shader_test.mako
	templates/gen_variable_index_read_tests/helpers.mako)

# OpenCL Test generators
piglit_make_generated_tests(
	builtin_cl_int_tests.list
	gen_cl_int_builtins.py)
piglit_make_generated_tests(
	cl_store_tests.list
	gen_cl_store_tests.py)
piglit_make_generated_tests(
	builtin_cl_math_tests.list
	gen_cl_math_builtins.py)
piglit_make_generated_tests(
	builtin_cl_relational_tests.list
	gen_cl_relational_builtins.py)
piglit_make_generated_tests(
	builtin_cl_common_tests.list
	gen_cl_common_builtins.py)

# Create a custom target for generating OpenGL tests
# This is not added to the default target, instead it is added
# as a dependency of gen-tests if OpenGL or OpenGLES{2,3} tests
# are enabled
add_custom_target(gen-gl-tests
	DEPENDS builtin_packing_tests.list
			outerproduct_tests.list
			outerproduct_invalid_params.list
			builtin_uniform_tests.list
			constant_array_size_tests.list
			const_builtin_equal_tests.list
			builtin_packing_tests.list
			interpolation_tests.list
			non-lvalue_tests.list
			texture_query_lod_tests.list
			texture_lod_tests.list
			shader_bit_encoding_tests.list
			uniform-initializer_tests.list
			interpolation-qualifier-built-in-variable.list
			builtin_uniform_tests_fp64.list
			constant_array_size_tests_fp64.list
			shader_precision_tests.list
			shader_image_load_store_tests.list
			variable_index_read_tests.list
)

# Create a custom target for generating OpenCL tests
# This is not added to the default target, instead it is added
# as a dependency of gen-tests if OpenCL tests are enabled
add_custom_target(gen-cl-tests
	DEPENDS builtin_cl_int_tests.list
			builtin_cl_math_tests.list
			builtin_cl_relational_tests.list
			builtin_cl_common_tests.list
			cl_store_tests.list
)

# Add a "gen-tests" target that can be used to generate all the
# tests without doing any other compilation.
add_custom_target(gen-tests ALL COMMENT "meta-target for other test generators")

# Add gen-gl-tests as a dependency of the gen-tests if OpenGL or OpenGL ES {2,3}
# are requested. there are no GLES1 tests to generate
if(${PIGLIT_BUILD_GL_TESTS} OR ${PIGLIT_BUILD_GLES2_TESTS} OR ${PIGLIT_BUILD_GLES3_TESTS})
	add_dependencies(gen-tests gen-gl-tests)
endif(${PIGLIT_BUILD_GL_TESTS} OR ${PIGLIT_BUILD_GLES2_TESTS} OR ${PIGLIT_BUILD_GLES3_TESTS})

# Add gen-cl-tests as a dependency of the gen-tests if OpenCL is requested
if(${PIGLIT_BUILD_CL_TESTS})
	add_dependencies(gen-tests gen-cl-tests)
endif(${PIGLIT_BUILD_CL_TESTS})
