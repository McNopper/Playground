#include "glsl_build.h"

#include <shaderc/shaderc.hpp>

std::optional<SpirvData> buildGlsl(const std::string& source, const std::map<std::string, std::string>& macros, SpvExecutionModel execution_model)
{
	shaderc_shader_kind shader_kind{};

	// Current execution models, which can be extended.
	switch (execution_model)
	{
	case SpvExecutionModelVertex:
		shader_kind = shaderc_vertex_shader;
		break;
	case SpvExecutionModelFragment:
		shader_kind = shaderc_fragment_shader;
		break;
	case SpvExecutionModelGeometry:
		shader_kind = shaderc_geometry_shader;
		break;
	case SpvExecutionModelTessellationControl:
		shader_kind = shaderc_tess_control_shader;
		break;
	case SpvExecutionModelTessellationEvaluation:
		shader_kind = shaderc_tess_evaluation_shader;
		break;
	case SpvExecutionModelGLCompute:
		shader_kind = shaderc_compute_shader;
		break;
	default:
		return {};
	}

	shaderc::Compiler compiler{};
	shaderc::CompileOptions compile_options{};
	
	for (const auto& it : macros)
	{
		if (it.second.empty())
		{
			compile_options.AddMacroDefinition(it.first);
		}
		else
		{
			compile_options.AddMacroDefinition(it.first, it.second);
		}
	}
	compile_options.SetOptimizationLevel(shaderc_optimization_level_zero);

	// Compile
	auto assembled = compiler.CompileGlslToSpv(source, shader_kind, "", compile_options);
	if (assembled.GetCompilationStatus() != shaderc_compilation_status_success)
	{
		return {};
	}

	SpirvData spirv_data{};
	spirv_data.code = { assembled.cbegin(), assembled.cend() };
	spirv_data.execution_model = execution_model;

	return spirv_data;
}
