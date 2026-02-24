
#ifndef CORE_H_
#define CORE_H_

// Layer

// templates

#include "templates/Filter.h"

// io

#include "io/binary_data.h"
#include "io/filesystem.h"

// math

#include "math/RandomGenerator.h"
#include "math/aabb.h"
#include "math/debug.h"
#include "math/frustum.h"
#include "math/helper.h"
#include "math/interpolate.h"
#include "math/matrix.h"
#include "math/matrix_camera.h"
#include "math/matrix_transform.h"
#include "math/plane.h"
#include "math/quaternion.h"
#include "math/quaternion_transform.h"
#include "math/real_spherical_harmonics.h"
#include "math/rsh_rotation.h"
#include "math/sphere.h"
#include "math/spherical_coordinate.h"
#include "math/vector.h"

// utility

#include "utility/DeltaTime.h"
#include "utility/base64.h"
#include "utility/convert.h"
#include "utility/generator.h"
#include "utility/gzip.h"
#include "utility/strings.h"

// Layer

// color

#include "color/cie_XYZ_cmf.h"
#include "color/convert.h"
#include "color/types.h"

// Layer

// image

#include "image/image_data.h"

// parser

#include "parser/parser.h"

#endif /* CORE_H_ */
