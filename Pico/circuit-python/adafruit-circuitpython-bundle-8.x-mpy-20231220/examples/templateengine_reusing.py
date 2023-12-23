# SPDX-FileCopyrightText: Copyright (c) 2023 Michał Pokusa
#
# SPDX-License-Identifier: Unlicense

from adafruit_templateengine import Template


template_string = r"""
<!DOCTYPE html>
<html>
    <head>
        <title>Hello</title>
    </head>
    <body>
        <p>Hello, {{ context.get("name") or "Anonymous User" }}!</p>
    </body>
</html>
"""

template = Template(template_string)

context = {"name": ""}  # Put your name here

print(template.render(context))
