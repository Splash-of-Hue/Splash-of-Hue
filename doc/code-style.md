# Code style...

This is a document describing the code style of Splash of Hue and is to be further defined here.

## Trello & TODOs & the code
Every TODO comment must have an associated Trello ID.

## Use of Auto
Please limit uses of auto keyword. Auto should be used when the type is obvious and known. If the type is complicated (say, std::map<std::string, std::vector<std::pair<int, float>>>), then auto should be used. If the type is simple (say, int or std::string), then auto should not be used.
Auto may also be used in a case where the returning function might change in the future, and so to avoid having to change the type later, auto may be used.

## Whitespace
Please limit whitespace to 1 whitespace between functions/lines and try to avoid unnecessary whitespace.  