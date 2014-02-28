import qbs.base 1.0
import qbs.TextFile
import qbs.FileInfo

Module {
    Depends { id: qtcore; name: "Qt.core" }

    additionalProductFileTags: qtcore.versionMajor < 5 ? ["pluginSpec"] : ["qt_plugin_metadata"]

    Rule {
        inputs: ["pluginSpecIn"]

        Artifact {
            fileTags: ["pluginSpec"]
            fileName: input.fileName.replace(/\.[^\.]*$/,'')
        }

        prepare: {
            var cmd = new JavaScriptCommand();
            cmd.description = "prepare " + FileInfo.fileName(output.fileName);
            cmd.highlight = "codegen";
            cmd.pluginspecreplacements = product.pluginspecreplacements;
            cmd.plugin_depends = [];
            var deps = product.dependencies;
            for (var d in deps) {
                var depdeps = deps[d].dependencies;
                for (var dd in depdeps) {
                    if (depdeps[dd].name == 'pluginspec') {
                        cmd.plugin_depends.push(deps[d].name);
                        break;
                    }
                }
            }
            cmd.plugin_recommends = product.pluginRecommends

            cmd.sourceCode = function() {
                var i;
                var vars = pluginspecreplacements || {};
                var inf = new TextFile(input.fileName);
                var all = inf.readAll();
                // replace quoted quotes
                all = all.replace(/\\\"/g, '"');
                // replace config vars
                vars['LISA_VERSION'] = project.lisa_version;
                vars['LISA_COMPAT_VERSION'] = project.lisa_compat_version;
                vars['LISA_VERSION_MAJOR'] = project.lisa_version_major;
                vars['LISA_VERSION_MINOR'] = project.lisa_version_minor;
                vars['LISA_VERSION_RELEASE'] = project.lisa_version_release;
                var deplist = ["<dependencyList>"];
                for (i in plugin_depends) {
                    deplist.push("        <dependency name=\"" + plugin_depends[i] + "\" version=\"" + project.lisa_version + "\"/>");
                }
                for (i in plugin_recommends) {
                    deplist.push("        <dependency name=\"" + plugin_recommends[i] + "\" version=\"" + project.lisa_version + "\" type=\"optional\"/>");
                }
                deplist.push("    </dependencyList>");
                vars['dependencyList'] = deplist.join("\n");
                for (i in vars) {
                    all = all.replace(new RegExp('\\\$\\\$' + i + '(?!\w)', 'g'), vars[i]);
                }
                var file = new TextFile(output.fileName, TextFile.WriteOnly);
                file.truncate();
                file.write(all);
                file.close();
            }
            return cmd;
        }
    }

    Rule {
        inputs: ["pluginSpec"]

        Artifact {
            fileTags: ["qt_plugin_metadata"]
            fileName: {
                var destdir = FileInfo.joinPaths(product.moduleProperty("Qt/core", "generatedFilesDir"),
                                                 input.fileName);
                return destdir.replace(/\.[^\.]*$/, '.json');
            }
        }

        prepare: {
            var xslFile = project.path + "/../pluginjsonmetadata.xsl"; // project is "Plugins"
            var xmlPatternsPath = product.moduleProperty("Qt/core", "binPath") + "/xmlpatterns";
            var args = [
                "-no-format",
                "-output",
                output.fileName,
                xslFile,
                input.fileName
            ];
            var cmd = new Command(xmlPatternsPath, args);
            cmd.description = "generating " + FileInfo.fileName(output.fileName);
            cmd.highlight = "codegen";
            return cmd;
        }
    }
}

