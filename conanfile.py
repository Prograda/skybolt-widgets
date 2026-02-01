from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, CMakeDeps
import os

class SkyboltWidgetsConan(ConanFile):
	implements = ["auto_shared_fpic"]
	name = "skybolt-widgets"
	version = "1.0.0"
	settings = "os", "compiler", "arch", "build_type"
	options = {
		"fPIC": [True, False],
		"shared": [True, False],
		"with_skybolt_reflect": [True, False]
	}
	default_options = {
		"fPIC": True,
		"shared": False,
		"with_skybolt_reflect": True
	}

	exports_sources = "*"
	no_copy_source = True

	def include_package(self, name, version, subfolder=None, transitive_headers=False):
		currentDir = os.path.dirname(os.path.abspath(__file__))
		recipes_path = os.path.join(currentDir, "Conan/Recipes", name)
		if (subfolder):
			recipes_path = os.path.join(recipes_path, subfolder)
            
		self.run(f"conan export --version {version} .", cwd=recipes_path)
		self.requires(f"{name}/{version}", transitive_headers=transitive_headers)

	def requirements(self):
		self.requires("catch2/2.13.8")
		self.requires("qt/6.10.1", transitive_headers=True)
		
		if self.options.with_skybolt_reflect:
			self.include_package("skybolt-reflect", "1.0.0", transitive_headers=True)

	def generate(self):
		tc = CMakeToolchain(self)
	
		tc.variables["BUILD_WITH_SKYBOLT_REFLECT"] = self.options.with_skybolt_reflect
		
		tc.generate()

		deps = CMakeDeps(self)
		deps.generate()

	def build(self):
		cmake = CMake(self)
		cmake.configure(variables={"CMAKE_INSTALL_PREFIX": f"{self.build_folder}/Install"})
		cmake.build()
		
	def package(self):
		cmake = CMake(self)
		cmake.install()
		
	def package_info(self):
		self.cpp_info.includedirs = ["include"]
		self.cpp_info.libs = ["SkyboltWidgets"]
		self.cpp_info.builddirs = ["cmake"]
		if self.options.with_skybolt_reflect:
			self.cpp_info.defines.append("BUILD_WITH_SKYBOLT_REFLECT=1")
