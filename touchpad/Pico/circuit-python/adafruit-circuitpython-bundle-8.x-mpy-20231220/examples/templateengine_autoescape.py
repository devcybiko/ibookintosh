# SPDX-FileCopyrightText: Copyright (c) 2023 Michał Pokusa
#
# SPDX-License-Identifier: Unlicense

from adafruit_templateengine import render_template, Language

# By default autoescape is enabled for HTML
print("HTML autoescape:")
print(render_template("./examples/autoescape.html"))

# But XML and Markdown are also supported
print("Markdown autoescape:")
print(render_template("./examples/autoescape.md", language=Language.MARKDOWN))
