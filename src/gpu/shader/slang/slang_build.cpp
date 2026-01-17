#include "slang_build.h"

#include <cstdio>
#include <cstring>
#include <utility>

#include <slang/slang.h>
#include <slang/slang-com-ptr.h>

std::vector<SpirvData> buildSlang(const std::string& filename, const std::map<std::string, std::string>& macros, const std::string& include_path)
{
	std::vector<SpirvData> shaders{};

	// Create global session
	Slang::ComPtr<slang::IGlobalSession> global_session{};

	auto result = slang::createGlobalSession(global_session.writeRef());
	if (SLANG_FAILED(result))
	{
		return shaders;
	}

	// Build target is latest SPIRV version
	slang::TargetDesc target_desc{};
	target_desc.format = SLANG_SPIRV;
	target_desc.profile = global_session->findProfile("spirv_1_6");
	if (target_desc.profile == SLANG_PROFILE_UNKNOWN)
	{
		return shaders;
	}

	// Gather compiler options
	std::vector<slang::CompilerOptionEntry> compiler_options{};
	// Generate SPRIV directly
	compiler_options.push_back(
		{
			slang::CompilerOptionName::EmitSpirvDirectly,
			{ slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr }
		}
	);
	// We use column major order as default
	compiler_options.push_back(
		{
			slang::CompilerOptionName::MatrixLayoutColumn,
			{ slang::CompilerOptionValueKind::Int, 1, 0, nullptr, nullptr },
		}
	);

	// Add macros
	for (const auto& it : macros)
	{
		compiler_options.push_back(
			{
				slang::CompilerOptionName::MacroDefine,
				{ slang::CompilerOptionValueKind::String, 0, 0, it.first.c_str(), it.second.c_str() }
			}
		);
	}

	// Use the resource shaders folder
	if (!include_path.empty())
	{
		compiler_options.push_back(
			{
				slang::CompilerOptionName::Include,
				{ slang::CompilerOptionValueKind::String, 0, 0, include_path.c_str(), nullptr }
			}
		);
	}

	slang::SessionDesc session_desc{};
	// Set the build target.
	session_desc.targets = &target_desc;
	session_desc.targetCount = 1;
	// Set the compiler options.
	session_desc.compilerOptionEntries = compiler_options.data();
	session_desc.compilerOptionEntryCount = (uint32_t)compiler_options.size();

	// Create build session.
	Slang::ComPtr<slang::ISession> session{};
	result = global_session->createSession(session_desc, session.writeRef());
	if (SLANG_FAILED(result))
	{
		return shaders;
	}

	Slang::ComPtr<slang::IBlob> diagnostics_blob{};

	Slang::ComPtr<slang::IModule> module{};
	module = session->loadModule(filename.c_str(), diagnostics_blob.writeRef());
	if (!module)
	{
		printf("%s\n", (const char*)diagnostics_blob->getBufferPointer());

		return shaders;
	}

	// Current execution models, which can be extended.
	std::vector<std::pair<std::string, SpvExecutionModel>> entry_point_checks{
		{ "vertexMain", SpvExecutionModelVertex },
		{ "tessellationControlMain", SpvExecutionModelTessellationControl },
		{ "tessellationEvaluationMain", SpvExecutionModelTessellationEvaluation },
		{ "geometryMain", SpvExecutionModelGeometry },
		{ "fragmentMain", SpvExecutionModelFragment },
		{ "computeMain", SpvExecutionModelGLCompute }
	};

	for (const auto& entry_point_check : entry_point_checks)
	{
		Slang::ComPtr<slang::IEntryPoint> entry_point{};
		
		result = module->findEntryPointByName(entry_point_check.first.c_str(), entry_point.writeRef());
		if (SLANG_FAILED(result))
		{
			continue;
		}

		// Slang contains given entry point and shader stage.

		std::vector<slang::IComponentType*> component_types{};
		component_types.push_back(module);
		component_types.push_back(entry_point);

		Slang::ComPtr<slang::IComponentType> composed_program{};		
		result = session->createCompositeComponentType(component_types.data(), component_types.size(), composed_program.writeRef(), diagnostics_blob.writeRef());
		if (SLANG_FAILED(result))
		{
			printf("%s\n", (const char*)diagnostics_blob->getBufferPointer());

			shaders.clear();
			return shaders;
		}

		Slang::ComPtr<slang::IComponentType> linked_program{};
		result = composed_program->link(linked_program.writeRef(), diagnostics_blob.writeRef());
		if (SLANG_FAILED(result))
		{
			printf("%s\n", (const char*)diagnostics_blob->getBufferPointer());

			shaders.clear();
			return shaders;
		}

		Slang::ComPtr<slang::IBlob> spirv_code{};
		result = linked_program->getEntryPointCode(0, 0, spirv_code.writeRef(), diagnostics_blob.writeRef());
		if (SLANG_FAILED(result))
		{
			printf("%s\n", (const char*)diagnostics_blob->getBufferPointer());

			shaders.clear();
			return shaders;
		}

		// Store spirv in shaders vector.

		SpirvData spirv_data{};
		spirv_data.execution_model = entry_point_check.second;

		spirv_data.code.resize(spirv_code->getBufferSize() / sizeof(uint32_t));
		std::memcpy(spirv_data.code.data(), spirv_code->getBufferPointer(), spirv_code->getBufferSize());
		
		shaders.push_back(spirv_data);
	}

	return shaders;
}
