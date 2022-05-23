#include "header1.hpp"

/*
* #include "header2.hpp"
#include "header2.hpp"
    #include "header2.hpp"
*/

//#include "header2.hpp"
R"(d
* #include "header2.hpp"
#include "header2.hpp"
    #include "header2.hpp"
)"
"#include \"header2.hpp\""

/**/   #include"subfolder2/header2.hpp"

#include                "not_existing_header.hpp"