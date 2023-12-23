# SPDX-FileCopyrightText: Copyright (c) 2023 Michał Pokusa
#
# SPDX-License-Identifier: Unlicense

## Example of autoescaping unsafe characters in Markdown

This would be a {{ "__bold text__" }}, but autoescaping is turned on, so all unsafe characters are escaped.

{% autoescape off %}
This is a {{ "__bold text__" }}, because autoescaping is turned off in this block.

{% autoescape on %}
And againg, this is not a {{ "__bold text__" }},
because in this block autoescaping is turned on again.
{% endautoescape %}

{% endautoescape %}
