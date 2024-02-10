#include <cstdio>
#include <cstdlib>
#include <climits>
#include <cmath>
#include <cstring>

#include "rasodmg/gmarray.hh"

using namespace std;

static const double radToDeg = 180.0 / M_PI;
static const double degToRad = M_PI / 180.0;
    
static const float ewres = 10.0f;
static const float nsres = -10.0f;
static const auto inv_ewres = 1.0 / ewres;

static const float z = 2.0f;
static const float scale = 0.5f;
static const float alt = 45.0f;
static const float az = 315.0f;
    
static const auto z_scaled = z / (8.0 * scale);
static const auto square_z = z_scaled * z_scaled;
static const auto sin_altRadians = sin(alt * degToRad);
static const auto sin_altRadians_mul_254 = 254.0 * sin_altRadians;
static const auto azRadians = az * degToRad;
    
static const auto cos_alt_mul_z = cos(alt * degToRad) * z_scaled;
static const auto cos_az_mul_cos_alt_mul_z = cos(azRadians) * cos_alt_mul_z;
static const auto sin_az_mul_cos_alt_mul_z = sin(azRadians) * cos_alt_mul_z;
static const auto cos_az_mul_cos_alt_mul_z_mul_254 = 254.0 * cos_az_mul_cos_alt_mul_z;
static const auto sin_az_mul_cos_alt_mul_z_mul_254 = 254.0 * sin_az_mul_cos_alt_mul_z;
static const auto square_z_mul_square_inv_res = square_z * inv_ewres * inv_ewres;
static const auto cos_az_mul_cos_alt_mul_z_mul_254_mul_inv_res = cos_az_mul_cos_alt_mul_z_mul_254 * -inv_ewres;
static const auto sin_az_mul_cos_alt_mul_z_mul_254_mul_inv_res = sin_az_mul_cos_alt_mul_z_mul_254 * inv_ewres;

extern "C" r_GMarray* hillshade(r_GMarray* arg)
{
    using T = char;
    const auto &dom = arg->spatial_domain();
    const auto x = dom[0].get_extent();
    const auto y = dom[1].get_extent();
    const auto size = dom.cell_count();
    
    
    // TODO: determine properly based on the array type
    float* src_array = reinterpret_cast<float*>(arg->get_array());
    char* res_array = (char*) malloc(size * sizeof(float));
    memset(res_array, 0, size);
    
    float* src_array_above = src_array;
    src_array = src_array + y;
    float* src_array_below = src_array + y;
    
    T* dst_array = ((T*)res_array) + y + 1;
    
    for (size_t i = 0; i < x - 1; ++i, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
    {
      for (size_t j = 0; j < y - 1; ++j, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
      {
        const auto a = src_array_above[0]; // 0
        const auto b = src_array_above[1]; // 1
        const auto c = src_array_above[2]; // 2
        const auto d = src_array[0];       // 3
        const auto f = src_array[2];       // 5
        const auto g = src_array_below[0]; // 6
        const auto h = src_array_below[1]; // 7
        const auto i = src_array_below[2]; // 8
        
        auto accX = a - i;
        auto accY = accX;
        const auto six_minus_two = g - c;
        const auto three_minus_five = d - f;
        const auto one_minus_seven = b - h;
        
        accX += three_minus_five;
        accY += one_minus_seven;
        accX += three_minus_five;
        accY += one_minus_seven;
        accX += six_minus_two;
        accY -= six_minus_two;
        const auto x = accX;
        const auto y = accY;
        
        const auto xx_plus_yy = x * x + y * y;
        
        // the shade value
        const auto aa = sin_altRadians_mul_254 +
          (x * sin_az_mul_cos_alt_mul_z_mul_254_mul_inv_res + 
           y * cos_az_mul_cos_alt_mul_z_mul_254_mul_inv_res);
        const auto bb = 1 + square_z_mul_square_inv_res * xx_plus_yy;
        const auto cang_mul_254 = aa / sqrt(bb);
        const auto cang = cang_mul_254 <= 0.0 ? 1.0 : 1.0 + cang_mul_254;
        
        *dst_array = static_cast<T>(cang);
      }
    }
    
    r_GMarray* ret = new r_GMarray();
    ret->set_array(res_array);
    ret->set_array_size(size * sizeof(T));
    ret->set_spatial_domain(dom);
    ret->set_type_length(sizeof(T));
    ret->set_type_structure("marray <char, 2>");
    ret->set_type_by_name("GreyImage");
    return ret;
}

extern "C" r_GMarray* aspect(r_GMarray* arg)
{
    const auto &dom = arg->spatial_domain();
    const auto x = dom[0].get_extent();
    const auto y = dom[1].get_extent();
    const auto size = dom.cell_count();
    
    const float ewres = 10.0;
    const float nsres = -10.0;
    
    // TODO: determine properly based on the array type
    float* src_array = reinterpret_cast<float*>(arg->get_array());
    char* res_array = (char*) malloc(size);
    memset(res_array, 0, size);
    
    float* src_array_above = src_array;
    src_array = src_array + y;
    float* src_array_below = src_array + y;
    
    char* dst_array = res_array + y + 1;
    
    for (size_t i = 0; i < x - 1; ++i, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
    {
      for (size_t j = 0; j < y - 1; ++j, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
      {
        const auto a = src_array_above[0]; // 0
        const auto b = src_array_above[1]; // 1
        const auto c = src_array_above[2]; // 2
        const auto d = src_array[0];       // 3
        const auto f = src_array[2];       // 5
        const auto g = src_array_below[0]; // 6
        const auto h = src_array_below[1]; // 7
        const auto i = src_array_below[2]; // 8
        
        const auto dx = ((c + f + f + i) - (a + d + d + g));
        const auto dy = ((g + h + h + i) - (a + b + b + c));
        
        auto aspect = static_cast<float>(atan2(dy, -dx) / degToRad);

        if (dx != 0 || dy != 0)
          aspect = (aspect > 90.0f) ? 450.0f - aspect : 90.0f - aspect;
        else
          aspect = 0.0;
        
        if (aspect == 360.0f)
          aspect = 0.0;
        
        *dst_array = static_cast<char>(aspect);
      }
    }
    
    r_GMarray* ret = new r_GMarray();
    ret->set_array(res_array);
    ret->set_array_size(size);
    ret->set_spatial_domain(dom);
    ret->set_type_length(sizeof(char));
    ret->set_type_structure("marray <char, 2>");
    ret->set_type_by_name("GreyImage");
    return ret;
}

extern "C" r_GMarray* slope(r_GMarray* arg)
{
    const auto &dom = arg->spatial_domain();
    const auto x = dom[0].get_extent();
    const auto y = dom[1].get_extent();
    const auto size = dom.cell_count();
    
    const float ewres = 10.0;
    const float nsres = -10.0;
    
    // TODO: determine properly based on the array type
    float* src_array = reinterpret_cast<float*>(arg->get_array());
    char* res_array = (char*) malloc(size);
    memset(res_array, 0, size);
    
    float* src_array_above = src_array;
    src_array = src_array + y;
    float* src_array_below = src_array + y;
    
    char* dst_array = res_array + y + 1;
    
    for (size_t i = 0; i < x - 1; ++i, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
    {
      for (size_t j = 0; j < y - 1; ++j, ++dst_array, ++src_array_above, ++src_array, ++src_array_below)
      {
        const auto a = src_array_above[0]; // 0
        const auto b = src_array_above[1]; // 1
        const auto c = src_array_above[2]; // 2
        const auto d = src_array[0];       // 3
        const auto f = src_array[2];       // 5
        const auto g = src_array_below[0]; // 6
        const auto h = src_array_below[1]; // 7
        const auto i = src_array_below[2]; // 8
        
        const auto dx = ((c + f + f + i) - (a + d + d + g)) / (8 * ewres);
        const auto dy = ((g + h + h + i) - (a + b + b + c)) / (8 * nsres);
        const auto key = (dx * dx + dy * dy);
        
        *dst_array = static_cast<char>(255.0 * atan(sqrt(key)));
      }
    }
    
    r_GMarray* ret = new r_GMarray();
    ret->set_array(res_array);
    ret->set_array_size(size);
    ret->set_spatial_domain(dom);
    ret->set_type_length(sizeof(char));
    ret->set_type_structure("marray <char, 2>");
    ret->set_type_by_name("GreyImage");
    return ret;
}