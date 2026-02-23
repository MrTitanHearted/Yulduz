#pragma once

#include <Yulduz/Engine/Entity.h>

typedef struct YULDUZ_ParentNode YULDUZ_ParentNode;
typedef struct YULDUZ_ChildNode  YULDUZ_ChildNode;

struct YULDUZ_ParentNode {
    YULDUZ_Entity FirstChild;
};

struct YULDUZ_ChildNode {
    YULDUZ_Entity ParentEntity;
    YULDUZ_Entity NextChild;
    YULDUZ_Entity PrevChild;  // LastChild.PrevChild.NextChild == FirstChild
};