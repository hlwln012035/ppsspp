#include <cstring>

extern "C"
{
#include "ext/libkirk/kirk_engine.h"
}

#include "../../Globals.h"
#include "PrxDecrypter.h"

#define ROUNDUP16(x)  (((x)+15)&~15)

// Thank you PSARDUMPER & JPCSP keys

// PRXDecrypter 16-byte tag keys.
static const u8 keys260_0[] = {0xC3, 0x24, 0x89, 0xD3, 0x80, 0x87, 0xB2, 0x4E, 0x4C, 0xD7, 0x49, 0xE4, 0x9D, 0x1D, 0x34, 0xD1};
static const u8 keys260_1[] = {0xF3, 0xAC, 0x6E, 0x7C, 0x04, 0x0A, 0x23, 0xE7, 0x0D, 0x33, 0xD8, 0x24, 0x73, 0x39, 0x2B, 0x4A};
static const u8 keys260_2[] = {0x72, 0xB4, 0x39, 0xFF, 0x34, 0x9B, 0xAE, 0x82, 0x30, 0x34, 0x4A, 0x1D, 0xA2, 0xD8, 0xB4, 0x3C};
static const u8 keys280_0[] = {0xCA, 0xFB, 0xBF, 0xC7, 0x50, 0xEA, 0xB4, 0x40, 0x8E, 0x44, 0x5C, 0x63, 0x53, 0xCE, 0x80, 0xB1};
static const u8 keys280_1[] = {0x40, 0x9B, 0xC6, 0x9B, 0xA9, 0xFB, 0x84, 0x7F, 0x72, 0x21, 0xD2, 0x36, 0x96, 0x55, 0x09, 0x74};
static const u8 keys280_2[] = {0x03, 0xA7, 0xCC, 0x4A, 0x5B, 0x91, 0xC2, 0x07, 0xFF, 0xFC, 0x26, 0x25, 0x1E, 0x42, 0x4B, 0xB5};
static const u8 keys300_0[] = {0x9F, 0x67, 0x1A, 0x7A, 0x22, 0xF3, 0x59, 0x0B, 0xAA, 0x6D, 0xA4, 0xC6, 0x8B, 0xD0, 0x03, 0x77};
static const u8 keys300_1[] = {0x15, 0x07, 0x63, 0x26, 0xDB, 0xE2, 0x69, 0x34, 0x56, 0x08, 0x2A, 0x93, 0x4E, 0x4B, 0x8A, 0xB2};
static const u8 keys300_2[] = {0x56, 0x3B, 0x69, 0xF7, 0x29, 0x88, 0x2F, 0x4C, 0xDB, 0xD5, 0xDE, 0x80, 0xC6, 0x5C, 0xC8, 0x73};
static const u8 keys303_0[] = {0x7b, 0xa1, 0xe2, 0x5a, 0x91, 0xb9, 0xd3, 0x13, 0x77, 0x65, 0x4a, 0xb7, 0xc2, 0x8a, 0x10, 0xaf};
static const u8 keys310_0[] = {0xa2, 0x41, 0xe8, 0x39, 0x66, 0x5b, 0xfa, 0xbb, 0x1b, 0x2d, 0x6e, 0x0e, 0x33, 0xe5, 0xd7, 0x3f};
static const u8 keys310_1[] = {0xA4, 0x60, 0x8F, 0xAB, 0xAB, 0xDE, 0xA5, 0x65, 0x5D, 0x43, 0x3A, 0xD1, 0x5E, 0xC3, 0xFF, 0xEA};
static const u8 keys310_2[] = {0xE7, 0x5C, 0x85, 0x7A, 0x59, 0xB4, 0xE3, 0x1D, 0xD0, 0x9E, 0xCE, 0xC2, 0xD6, 0xD4, 0xBD, 0x2B};
static const u8 keys310_3[] = {0x2E, 0x00, 0xF6, 0xF7, 0x52, 0xCF, 0x95, 0x5A, 0xA1, 0x26, 0xB4, 0x84, 0x9B, 0x58, 0x76, 0x2F};
static const u8 keys330_0[] = {0x3B, 0x9B, 0x1A, 0x56, 0x21, 0x80, 0x14, 0xED, 0x8E, 0x8B, 0x08, 0x42, 0xFA, 0x2C, 0xDC, 0x3A};
static const u8 keys330_1[] = {0xE8, 0xBE, 0x2F, 0x06, 0xB1, 0x05, 0x2A, 0xB9, 0x18, 0x18, 0x03, 0xE3, 0xEB, 0x64, 0x7D, 0x26};
static const u8 keys330_2[] = {0xAB, 0x82, 0x25, 0xD7, 0x43, 0x6F, 0x6C, 0xC1, 0x95, 0xC5, 0xF7, 0xF0, 0x63, 0x73, 0x3F, 0xE7};
static const u8 keys330_3[] = {0xA8, 0xB1, 0x47, 0x77, 0xDC, 0x49, 0x6A, 0x6F, 0x38, 0x4C, 0x4D, 0x96, 0xBD, 0x49, 0xEC, 0x9B};
static const u8 keys330_4[] = {0xEC, 0x3B, 0xD2, 0xC0, 0xFA, 0xC1, 0xEE, 0xB9, 0x9A, 0xBC, 0xFF, 0xA3, 0x89, 0xF2, 0x60, 0x1F};
static const u8 keys360_0[] = {0x3C, 0x2B, 0x51, 0xD4, 0x2D, 0x85, 0x47, 0xDA, 0x2D, 0xCA, 0x18, 0xDF, 0xFE, 0x54, 0x09, 0xED};
static const u8 keys360_1[] = {0x31, 0x1F, 0x98, 0xD5, 0x7B, 0x58, 0x95, 0x45, 0x32, 0xAB, 0x3A, 0xE3, 0x89, 0x32, 0x4B, 0x34};
static const u8 keys370_0[] = {0x26, 0x38, 0x0A, 0xAC, 0xA5, 0xD8, 0x74, 0xD1, 0x32, 0xB7, 0x2A, 0xBF, 0x79, 0x9E, 0x6D, 0xDB};
static const u8 keys370_1[] = {0x53, 0xE7, 0xAB, 0xB9, 0xC6, 0x4A, 0x4B, 0x77, 0x92, 0x17, 0xB5, 0x74, 0x0A, 0xDA, 0xA9, 0xEA};
static const u8 keys370_2[] = {0x71, 0x10, 0xF0, 0xA4, 0x16, 0x14, 0xD5, 0x93, 0x12, 0xFF, 0x74, 0x96, 0xDF, 0x1F, 0xDA, 0x89};
static const u8 keys390_0[] = {0x45, 0xEF, 0x5C, 0x5D, 0xED, 0x81, 0x99, 0x84, 0x12, 0x94, 0x8F, 0xAB, 0xE8, 0x05, 0x6D, 0x7D};
static const u8 keys390_1[] = {0x70, 0x1B, 0x08, 0x25, 0x22, 0xA1, 0x4D, 0x3B, 0x69, 0x21, 0xF9, 0x71, 0x0A, 0xA8, 0x41, 0xA9};
static const u8 keys500_0[] = {0xEB, 0x1B, 0x53, 0x0B, 0x62, 0x49, 0x32, 0x58, 0x1F, 0x83, 0x0A, 0xF4, 0x99, 0x3D, 0x75, 0xD0};
static const u8 keys500_1[] = {0xBA, 0xE2, 0xA3, 0x12, 0x07, 0xFF, 0x04, 0x1B, 0x64, 0xA5, 0x11, 0x85, 0xF7, 0x2F, 0x99, 0x5B};
static const u8 keys500_2[] = {0x2C, 0x8E, 0xAF, 0x1D, 0xFF, 0x79, 0x73, 0x1A, 0xAD, 0x96, 0xAB, 0x09, 0xEA, 0x35, 0x59, 0x8B};
static const u8 keys500_c[] = {0xA3, 0x5D, 0x51, 0xE6, 0x56, 0xC8, 0x01, 0xCA, 0xE3, 0x77, 0xBF, 0xCD, 0xFF, 0x24, 0xDA, 0x4D};
static const u8 keys505_a[] = {0x7B, 0x94, 0x72, 0x27, 0x4C, 0xCC, 0x54, 0x3B, 0xAE, 0xDF, 0x46, 0x37, 0xAC, 0x01, 0x4D, 0x87};
static const u8 keys505_0[] = {0x2E, 0x8E, 0x97, 0xA2, 0x85, 0x42, 0x70, 0x73, 0x18, 0xDA, 0xA0, 0x8A, 0xF8, 0x62, 0xA2, 0xB0};
static const u8 keys505_1[] = {0x58, 0x2A, 0x4C, 0x69, 0x19, 0x7B, 0x83, 0x3D, 0xD2, 0x61, 0x61, 0xFE, 0x14, 0xEE, 0xAA, 0x11};
static const u8 keys570_5k[] = {0x6D, 0x72, 0xA4, 0xBA, 0x7F, 0xBF, 0xD1, 0xF1, 0xA9, 0xF3, 0xBB, 0x07, 0x1B, 0xC0, 0xB3, 0x66};
static const u8 keys600_1[] = {0xE3, 0x52, 0x39, 0x97, 0x3B, 0x84, 0x41, 0x1C, 0xC3, 0x23, 0xF1, 0xB8, 0xA9, 0x09, 0x4B, 0xF0};
static const u8 keys600_2[] = {0xE1, 0x45, 0x93, 0x2C, 0x53, 0xE2, 0xAB, 0x06, 0x6F, 0xB6, 0x8F, 0x0B, 0x66, 0x91, 0xE7, 0x1E};
static const u8 keys620_0[] = {0xD6, 0xBD, 0xCE, 0x1E, 0x12, 0xAF, 0x9A, 0xE6, 0x69, 0x30, 0xDE, 0xDA, 0x88, 0xB8, 0xFF, 0xFB};
static const u8 keys620_1[] = {0x1D, 0x13, 0xE9, 0x50, 0x04, 0x73, 0x3D, 0xD2, 0xE1, 0xDA, 0xB9, 0xC1, 0xE6, 0x7B, 0x25, 0xA7};
static const u8 keys620_a[] = {0xAC, 0x34, 0xBA, 0xB1, 0x97, 0x8D, 0xAE, 0x6F, 0xBA, 0xE8, 0xB1, 0xD6, 0xDF, 0xDF, 0xF1, 0xA2};
static const u8 keys620_e[] = {0xB1, 0xB3, 0x7F, 0x76, 0xC3, 0xFB, 0x88, 0xE6, 0xF8, 0x60, 0xD3, 0x35, 0x3C, 0xA3, 0x4E, 0xF3};
static const u8 keys620_5[] = {0xF1, 0xBC, 0x17, 0x07, 0xAE, 0xB7, 0xC8, 0x30, 0xD8, 0x34, 0x9D, 0x40, 0x6A, 0x8E, 0xDF, 0x4E};
static const u8 keys620_5k[] = {0x41, 0x8A, 0x35, 0x4F, 0x69, 0x3A, 0xDF, 0x04, 0xFD, 0x39, 0x46, 0xA2, 0x5C, 0x2D, 0xF2, 0x21};
static const u8 keys620_5v[] = {0xF2, 0x8F, 0x75, 0xA7, 0x31, 0x91, 0xCE, 0x9E, 0x75, 0xBD, 0x27, 0x26, 0xB4, 0xB4, 0x0C, 0x32};
static const u8 keys630_k1[] = {0x36, 0xB0, 0xDC, 0xFC, 0x59, 0x2A, 0x95, 0x1D, 0x80, 0x2D, 0x80, 0x3F, 0xCD, 0x30, 0xA0, 0x1B};
static const u8 keys630_k2[] = {0xd4, 0x35, 0x18, 0x02, 0x29, 0x68, 0xfb, 0xa0, 0x6a, 0xa9, 0xa5, 0xed, 0x78, 0xfd, 0x2e, 0x9d};
static const u8 keys630_k3[] = {0x23, 0x8D, 0x3D, 0xAE, 0x41, 0x50, 0xA0, 0xFA, 0xF3, 0x2F, 0x32, 0xCE, 0xC7, 0x27, 0xCD, 0x50};
static const u8 keys630_k4[] = {0xAA, 0xA1, 0xB5, 0x7C, 0x93, 0x5A, 0x95, 0xBD, 0xEF, 0x69, 0x16, 0xFC, 0x2B, 0x92, 0x31, 0xDD};
static const u8 keys630_k5[] = {0x87, 0x37, 0x21, 0xCC, 0x65, 0xAE, 0xAA, 0x5F, 0x40, 0xF6, 0x6F, 0x2A, 0x86, 0xC7, 0xA1, 0xC8};
static const u8 keys630_k6[] = {0x8D, 0xDB, 0xDC, 0x5C, 0xF2, 0x70, 0x2B, 0x40, 0xB2, 0x3D, 0x00, 0x09, 0x61, 0x7C, 0x10, 0x60};
static const u8 keys630_k7[] = {0x77, 0x1C, 0x06, 0x5F, 0x53, 0xEC, 0x3F, 0xFC, 0x22, 0xCE, 0x5A, 0x27, 0xFF, 0x78, 0xA8, 0x48};
static const u8 keys630_k8[] = {0x81, 0xD1, 0x12, 0x89, 0x35, 0xC8, 0xEA, 0x8B, 0xE0, 0x02, 0x2D, 0x2D, 0x6A, 0x18, 0x67, 0xB8};
static const u8 keys636_k1[] = {0x07, 0xE3, 0x08, 0x64, 0x7F, 0x60, 0xA3, 0x36, 0x6A, 0x76, 0x21, 0x44, 0xC9, 0xD7, 0x06, 0x83};
static const u8 keys636_k2[] = {0x91, 0xF2, 0x02, 0x9E, 0x63, 0x32, 0x30, 0xA9, 0x1D, 0xDA, 0x0B, 0xA8, 0xB7, 0x41, 0xA3, 0xCC};
static const u8 keys638_k4[] = {0x98, 0x43, 0xFF, 0x85, 0x68, 0xB2, 0xDB, 0x3B, 0xD4, 0x22, 0xD0, 0x4F, 0xAB, 0x5F, 0x0A, 0x31};
static const u8 keys639_k3[] = {0x01, 0x7B, 0xF0, 0xE9, 0xBE, 0x9A, 0xDD, 0x54, 0x37, 0xEA, 0x0E, 0xC4, 0xD6, 0x4D, 0x8E, 0x9E};
static const u8 keys660_k1[] = {0x76, 0xF2, 0x6C, 0x0A, 0xCA, 0x3A, 0xBA, 0x4E, 0xAC, 0x76, 0xD2, 0x40, 0xF5, 0xC3, 0xBF, 0xF9};
static const u8 keys660_k2[] = {0x7A, 0x3E, 0x55, 0x75, 0xB9, 0x6A, 0xFC, 0x4F, 0x3E, 0xE3, 0xDF, 0xB3, 0x6C, 0xE8, 0x2A, 0x82};
static const u8 keys660_k3[] = {0xFA, 0x79, 0x09, 0x36, 0xE6, 0x19, 0xE8, 0xA4, 0xA9, 0x41, 0x37, 0x18, 0x81, 0x02, 0xE9, 0xB3};
static const u8 keys660_v1[] = {0xBA, 0x76, 0x61, 0x47, 0x8B, 0x55, 0xA8, 0x72, 0x89, 0x15, 0x79, 0x6D, 0xD7, 0x2F, 0x78, 0x0E};
static const u8 keys660_v2[] = {0xF9, 0x4A, 0x6B, 0x96, 0x79, 0x3F, 0xEE, 0x0A, 0x04, 0xC8, 0x8D, 0x7E, 0x5F, 0x38, 0x3A, 0xCF};
static const u8 keys660_v3[] = {0x88, 0xAF, 0x18, 0xE9, 0xC3, 0xAA, 0x6B, 0x56, 0xF7, 0xC5, 0xA8, 0xBF, 0x1A, 0x84, 0xE9, 0xF3};
static const u8 keys660_v4[] = {0xD1, 0xB0, 0xAE, 0xC3, 0x24, 0x36, 0x13, 0x49, 0xD6, 0x49, 0xD7, 0x88, 0xEA, 0xA4, 0x99, 0x86};
static const u8 keys660_v5[] = {0xCB, 0x93, 0x12, 0x38, 0x31, 0xC0, 0x2D, 0x2E, 0x7A, 0x18, 0x5C, 0xAC, 0x92, 0x93, 0xAB, 0x32};
static const u8 keys660_v6[] = {0x92, 0x8C, 0xA4, 0x12, 0xD6, 0x5C, 0x55, 0x31, 0x5B, 0x94, 0x23, 0x9B, 0x62, 0xB3, 0xDB, 0x47};
static const u8 keys660_k4[] = {0xC8, 0xA0, 0x70, 0x98, 0xAE, 0xE6, 0x2B, 0x80, 0xD7, 0x91, 0xE6, 0xCA, 0x4C, 0xA9, 0x78, 0x4E};
static const u8 keys660_k5[] = {0xBF, 0xF8, 0x34, 0x02, 0x84, 0x47, 0xBD, 0x87, 0x1C, 0x52, 0x03, 0x23, 0x79, 0xBB, 0x59, 0x81};
static const u8 keys660_k6[] = {0xD2, 0x83, 0xCC, 0x63, 0xBB, 0x10, 0x15, 0xE7, 0x7B, 0xC0, 0x6D, 0xEE, 0x34, 0x9E, 0x4A, 0xFA};
static const u8 keys660_k7[] = {0xEB, 0xD9, 0x1E, 0x05, 0x3C, 0xAE, 0xAB, 0x62, 0xE3, 0xB7, 0x1F, 0x37, 0xE5, 0xCD, 0x68, 0xC3};
static const u8 keys660_v7[] = {0xC5, 0x9C, 0x77, 0x9C, 0x41, 0x01, 0xE4, 0x85, 0x79, 0xC8, 0x71, 0x63, 0xA5, 0x7D, 0x4F, 0xFB};
static const u8 keys660_v8[] = {0x86, 0xA0, 0x7D, 0x4D, 0xB3, 0x6B, 0xA2, 0xFD, 0xF4, 0x15, 0x85, 0x70, 0x2D, 0x6A, 0x0D, 0x3A};
static const u8 keys660_k8[] = {0x85, 0x93, 0x1F, 0xED, 0x2C, 0x4D, 0xA4, 0x53, 0x59, 0x9C, 0x3F, 0x16, 0xF3, 0x50, 0xDE, 0x46};
static const u8 key_21C0[] = {0x6A, 0x19, 0x71, 0xF3, 0x18, 0xDE, 0xD3, 0xA2, 0x6D, 0x3B, 0xDE, 0xC7, 0xBE, 0x98, 0xE2, 0x4C};
static const u8 key_2250[] = {0x50, 0xCC, 0x03, 0xAC, 0x3F, 0x53, 0x1A, 0xFA, 0x0A, 0xA4, 0x34, 0x23, 0x86, 0x61, 0x7F, 0x97};
static const u8 key_22E0[] = {0x66, 0x0F, 0xCB, 0x3B, 0x30, 0x75, 0xE3, 0x10, 0x0A, 0x95, 0x65, 0xC7, 0x3C, 0x93, 0x87, 0x22};
static const u8 key_2D80[] = {0x40, 0x02, 0xC0, 0xBF, 0x20, 0x02, 0xC0, 0xBF, 0x5C, 0x68, 0x2B, 0x95, 0x5F, 0x40, 0x7B, 0xB8};
static const u8 key_2D90[] = {0x55, 0x19, 0x35, 0x10, 0x48, 0xD8, 0x2E, 0x46, 0xA8, 0xB1, 0x47, 0x77, 0xDC, 0x49, 0x6A, 0x6F};
static const u8 key_2DA8[] = {0x80, 0x02, 0xC0, 0xBF, 0x00, 0x0A, 0xC0, 0xBF, 0x40, 0x03, 0xC0, 0xBF, 0x40, 0x00, 0x00, 0x00};
static const u8 key_2DB8[] = {0x4C, 0x2D, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xB8, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static const u8 key_D91605F0[] = {0xB8, 0x8C, 0x45, 0x8B, 0xB6, 0xE7, 0x6E, 0xB8, 0x51, 0x59, 0xA6, 0x53, 0x7C, 0x5E, 0x86, 0x31};
static const u8 key_D91606F0[] = {0xED, 0x10, 0xE0, 0x36, 0xC4, 0xFE, 0x83, 0xF3, 0x75, 0x70, 0x5E, 0xF6, 0xA4, 0x40, 0x05, 0xF7};
static const u8 key_D91608F0[] = {0x5C, 0x77, 0x0C, 0xBB, 0xB4, 0xC2, 0x4F, 0xA2, 0x7E, 0x3B, 0x4E, 0xB4, 0xB4, 0xC8, 0x70, 0xAF};
static const u8 key_D91609F0[] = {0xD0, 0x36, 0x12, 0x75, 0x80, 0x56, 0x20, 0x43, 0xC4, 0x30, 0x94, 0x3E, 0x1C, 0x75, 0xD1, 0xBF};
static const u8 key_D9160AF0[] = {0x10, 0xA9, 0xAC, 0x16, 0xAE, 0x19, 0xC0, 0x7E, 0x3B, 0x60, 0x77, 0x86, 0x01, 0x6F, 0xF2, 0x63};
static const u8 key_D9160BF0[] = {0x83, 0x83, 0xF1, 0x37, 0x53, 0xD0, 0xBE, 0xFC, 0x8D, 0xA7, 0x32, 0x52, 0x46, 0x0A, 0xC2, 0xC2};
static const u8 key_D91611F0[] = {0x61, 0xB0, 0xC0, 0x58, 0x71, 0x57, 0xD9, 0xFA, 0x74, 0x67, 0x0E, 0x5C, 0x7E, 0x6E, 0x95, 0xB9};
static const u8 key_D91612F0[] = {0x9E, 0x20, 0xE1, 0xCD, 0xD7, 0x88, 0xDE, 0xC0, 0x31, 0x9B, 0x10, 0xAF, 0xC5, 0xB8, 0x73, 0x23};
static const u8 key_D91613F0[] = {0xEB, 0xFF, 0x40, 0xD8, 0xB4, 0x1A, 0xE1, 0x66, 0x91, 0x3B, 0x8F, 0x64, 0xB6, 0xFC, 0xB7, 0x12};
static const u8 key_D91614F0[] = {0xFD, 0xF7, 0xB7, 0x3C, 0x9F, 0xD1, 0x33, 0x95, 0x11, 0xB8, 0xB5, 0xBB, 0x54, 0x23, 0x73, 0x85};
static const u8 key_D91615F0[] = {0xC8, 0x03, 0xE3, 0x44, 0x50, 0xF1, 0xE7, 0x2A, 0x6A, 0x0D, 0xC3, 0x61, 0xB6, 0x8E, 0x5F, 0x51};
static const u8 key_D91616F0[] = {0x53, 0x03, 0xB8, 0x6A, 0x10, 0x19, 0x98, 0x49, 0x1C, 0xAF, 0x30, 0xE4, 0x25, 0x1B, 0x6B, 0x28};
static const u8 key_D91617F0[] = {0x02, 0xFA, 0x48, 0x73, 0x75, 0xAF, 0xAE, 0x0A, 0x67, 0x89, 0x2B, 0x95, 0x4B, 0x09, 0x87, 0xA3};
static const u8 key_D91618F0[] = {0x96, 0x96, 0x7C, 0xC3, 0xF7, 0x12, 0xDA, 0x62, 0x1B, 0xF6, 0x9A, 0x9A, 0x44, 0x44, 0xBC, 0x48};
static const u8 key_D91619F0[] = {0xE0, 0x32, 0xA7, 0x08, 0x6B, 0x2B, 0x29, 0x2C, 0xD1, 0x4D, 0x5B, 0xEE, 0xA8, 0xC8, 0xB4, 0xE9};
static const u8 key_D9161AF0[] = {0x27, 0xE5, 0xA7, 0x49, 0x52, 0xE1, 0x94, 0x67, 0x35, 0x66, 0x91, 0x0C, 0xE8, 0x9A, 0x25, 0x24};
static const u8 key_D91620F0[] = {0x52, 0x1C, 0xB4, 0x5F, 0x40, 0x3B, 0x9A, 0xDD, 0xAC, 0xFC, 0xEA, 0x92, 0xFD, 0xDD, 0xF5, 0x90};
static const u8 key_D91621F0[] = {0xD1, 0x91, 0x2E, 0xA6, 0x21, 0x14, 0x29, 0x62, 0xF6, 0xED, 0xAE, 0xCB, 0xDD, 0xA3, 0xBA, 0xFE};
static const u8 key_D91622F0[] = {0x59, 0x5D, 0x78, 0x4D, 0x21, 0xB2, 0x01, 0x17, 0x6C, 0x9A, 0xB5, 0x1B, 0xDA, 0xB7, 0xF9, 0xE6};
static const u8 key_D91623F0[] = {0xAA, 0x45, 0xEB, 0x4F, 0x62, 0xFB, 0xD1, 0x0D, 0x71, 0xD5, 0x62, 0xD2, 0xF5, 0xBF, 0xA5, 0x2F};
static const u8 key_D91624F0[] = {0x61, 0xB7, 0x26, 0xAF, 0x8B, 0xF1, 0x41, 0x58, 0x83, 0x6A, 0xC4, 0x92, 0x12, 0xCB, 0xB1, 0xE9};
static const u8 key_D91628F0[] = {0x49, 0xA4, 0xFC, 0x66, 0xDC, 0xE7, 0x62, 0x21, 0xDB, 0x18, 0xA7, 0x50, 0xD6, 0xA8, 0xC1, 0xB6};
static const u8 key_D91680F0[] = {0x2C, 0x22, 0x9B, 0x12, 0x36, 0x74, 0x11, 0x67, 0x49, 0xD1, 0xD1, 0x88, 0x92, 0xF6, 0xA1, 0xD8};
static const u8 key_D91681F0[] = {0x52, 0xB6, 0x36, 0x6C, 0x8C, 0x46, 0x7F, 0x7A, 0xCC, 0x11, 0x62, 0x99, 0xC1, 0x99, 0xBE, 0x98};
static const u8 key_2E5E10F0[] = {0x9D, 0x5C, 0x5B, 0xAF, 0x8C, 0xD8, 0x69, 0x7E, 0x51, 0x9F, 0x70, 0x96, 0xE6, 0xD5, 0xC4, 0xE8};
static const u8 key_2E5E12F0[] = {0x8A, 0x7B, 0xC9, 0xD6, 0x52, 0x58, 0x88, 0xEA, 0x51, 0x83, 0x60, 0xCA, 0x16, 0x79, 0xE2, 0x07};
static const u8 key_2E5E13F0[] = {0xFF, 0xA4, 0x68, 0xC3, 0x31, 0xCA, 0xB7, 0x4C, 0xF1, 0x23, 0xFF, 0x01, 0x65, 0x3D, 0x26, 0x36};
static const u8 key_2FD30BF0[] = {0xD8, 0x58, 0x79, 0xF9, 0xA4, 0x22, 0xAF, 0x86, 0x90, 0xAC, 0xDA, 0x45, 0xCE, 0x60, 0x40, 0x3F};
static const u8 key_2FD311F0[] = {0x3A, 0x6B, 0x48, 0x96, 0x86, 0xA5, 0xC8, 0x80, 0x69, 0x6C, 0xE6, 0x4B, 0xF6, 0x04, 0x17, 0x44};
static const u8 key_2FD312F0[] = {0xC5, 0xFB, 0x69, 0x03, 0x20, 0x7A, 0xCF, 0xBA, 0x2C, 0x90, 0xF8, 0xB8, 0x4D, 0xD2, 0xF1, 0xDE};
static const u8 keys02G_E[] = {0x9D, 0x09, 0xFD, 0x20, 0xF3, 0x8F, 0x10, 0x69, 0x0D, 0xB2, 0x6F, 0x00, 0xCC, 0xC5, 0x51, 0x2E};
static const u8 keys03G_E[] = {0x4F, 0x44, 0x5C, 0x62, 0xB3, 0x53, 0xC4, 0x30, 0xFC, 0x3A, 0xA4, 0x5B, 0xEC, 0xFE, 0x51, 0xEA};
static const u8 keys05G_E[] = {0x5D, 0xAA, 0x72, 0xF2, 0x26, 0x60, 0x4D, 0x1C, 0xE7, 0x2D, 0xC8, 0xA3, 0x2F, 0x79, 0xC5, 0x54};
static const u8 oneseg_310[] = {0xC7, 0x27, 0x72, 0x85, 0xAB, 0xA7, 0xF7, 0xF0, 0x4C, 0xC1, 0x86, 0xCC, 0xE3, 0x7F, 0x17, 0xCA};
static const u8 oneseg_300[] = {0x76, 0x40, 0x9E, 0x08, 0xDB, 0x9B, 0x3B, 0xA1, 0x47, 0x8A, 0x96, 0x8E, 0xF3, 0xF7, 0x62, 0x92};
static const u8 oneseg_280[] = {0x23, 0xDC, 0x3B, 0xB5, 0xA9, 0x82, 0xD6, 0xEA, 0x63, 0xA3, 0x6E, 0x2B, 0x2B, 0xE9, 0xE1, 0x54};
static const u8 oneseg_260_271[] = {0x22, 0x43, 0x57, 0x68, 0x2F, 0x41, 0xCE, 0x65, 0x4C, 0xA3, 0x7C, 0xC6, 0xC4, 0xAC, 0xF3, 0x60};
static const u8 oneseg_slim[] = {0x12, 0x57, 0x0D, 0x8A, 0x16, 0x6D, 0x87, 0x06, 0x03, 0x7D, 0xC8, 0x8B, 0x62, 0xA3, 0x32, 0xA9};
static const u8 ms_app_main[] = {0x1E, 0x2E, 0x38, 0x49, 0xDA, 0xD4, 0x16, 0x08, 0x27, 0x2E, 0xF3, 0xBC, 0x37, 0x75, 0x80, 0x93};
static const u8 demokeys_280[] = {0x12, 0x99, 0x70, 0x5E, 0x24, 0x07, 0x6C, 0xD0, 0x2D, 0x06, 0xFE, 0x7E, 0xB3, 0x0C, 0x11, 0x26};
static const u8 demokeys_3XX_1[] = {0x47, 0x05, 0xD5, 0xE3, 0x56, 0x1E, 0x81, 0x9B, 0x09, 0x2F, 0x06, 0xDB, 0x6B, 0x12, 0x92, 0xE0};
static const u8 demokeys_3XX_2[] = {0xF6, 0x62, 0x39, 0x6E, 0x26, 0x22, 0x4D, 0xCA, 0x02, 0x64, 0x16, 0x99, 0x7B, 0x9A, 0xE7, 0xB8};
static const u8 ebootbin_271_new[] = {0xF4, 0xAE, 0xF4, 0xE1, 0x86, 0xDD, 0xD2, 0x9C, 0x7C, 0xC5, 0x42, 0xA6, 0x95, 0xA0, 0x83, 0x88};
static const u8 ebootbin_280_new[] = {0xB8, 0x8C, 0x45, 0x8B, 0xB6, 0xE7, 0x6E, 0xB8, 0x51, 0x59, 0xA6, 0x53, 0x7C, 0x5E, 0x86, 0x31};
static const u8 ebootbin_300_new[] = {0xED, 0x10, 0xE0, 0x36, 0xC4, 0xFE, 0x83, 0xF3, 0x75, 0x70, 0x5E, 0xF6, 0xA4, 0x40, 0x05, 0xF7};
static const u8 ebootbin_310_new[] = {0x5C, 0x77, 0x0C, 0xBB, 0xB4, 0xC2, 0x4F, 0xA2, 0x7E, 0x3B, 0x4E, 0xB4, 0xB4, 0xC8, 0x70, 0xAF};
static const u8 gameshare_260_271[] = {0xF9, 0x48, 0x38, 0x0C, 0x96, 0x88, 0xA7, 0x74, 0x4F, 0x65, 0xA0, 0x54, 0xC2, 0x76, 0xD9, 0xB8};
static const u8 gameshare_280[] = {0x2D, 0x86, 0x77, 0x3A, 0x56, 0xA4, 0x4F, 0xDD, 0x3C, 0x16, 0x71, 0x93, 0xAA, 0x8E, 0x11, 0x43};
static const u8 gameshare_300[] = {0x78, 0x1A, 0xD2, 0x87, 0x24, 0xBD, 0xA2, 0x96, 0x18, 0x3F, 0x89, 0x36, 0x72, 0x90, 0x92, 0x85};
static const u8 gameshare_310[] = {0xC9, 0x7D, 0x3E, 0x0A, 0x54, 0x81, 0x6E, 0xC7, 0x13, 0x74, 0x99, 0x74, 0x62, 0x18, 0xE7, 0xDD};
static const u8 key_380210F0[] = {0x32, 0x2C, 0xFA, 0x75, 0xE4, 0x7E, 0x93, 0xEB, 0x9F, 0x22, 0x80, 0x85, 0x57, 0x08, 0x98, 0x48};
static const u8 key_380280F0[] = {0x97, 0x09, 0x12, 0xD3, 0xDB, 0x02, 0xBD, 0xD8, 0xE7, 0x74, 0x51, 0xFE, 0xF0, 0xEA, 0x6C, 0x5C};
static const u8 key_380283F0[] = {0x34, 0x20, 0x0C, 0x8E, 0xA1, 0x86, 0x79, 0x84, 0xAF, 0x13, 0xAE, 0x34, 0x77, 0x6F, 0xEA, 0x89};
static const u8 key_407810F0[] = {0xAF, 0xAD, 0xCA, 0xF1, 0x95, 0x59, 0x91, 0xEC, 0x1B, 0x27, 0xD0, 0x4E, 0x8A, 0xF3, 0x3D, 0xE7};
static const u8 drmkeys_6XX_1[] = {0x36, 0xEF, 0x82, 0x4E, 0x74, 0xFB, 0x17, 0x5B, 0x14, 0x14, 0x05, 0xF3, 0xB3, 0x8A, 0x76, 0x18};
static const u8 drmkeys_6XX_2[] = {0x21, 0x52, 0x5D, 0x76, 0xF6, 0x81, 0x0F, 0x15, 0x2F, 0x4A, 0x40, 0x89, 0x63, 0xA0, 0x10, 0x55};

// PRXDecrypter 144-byte tag keys.
static const u32 g_key0[] = {
		0x7b21f3be, 0x299c5e1d, 0x1c9c5e71, 0x96cb4645, 0x3c9b1be0, 0xeb85de3d,
		0x4a7f2022, 0xc2206eaa, 0xd50b3265, 0x55770567, 0x3c080840, 0x981d55f2,
		0x5fd8f6f3, 0xee8eb0c5, 0x944d8152, 0xf8278651, 0x2705bafa, 0x8420e533,
		0x27154ae9, 0x4819aa32, 0x59a3aa40, 0x2cb3cf65, 0xf274466d, 0x3a655605,
		0x21b0f88f, 0xc5b18d26, 0x64c19051, 0xd669c94e, 0xe87035f2, 0x9d3a5909,
		0x6f4e7102, 0xdca946ce, 0x8416881b, 0xbab097a5, 0x249125c6, 0xb34c0872};
static const u32 g_key2[] = {
		0xccfda932, 0x51c06f76, 0x046dcccf, 0x49e1821e, 0x7d3b024c, 0x9dda5865,
		0xcc8c9825, 0xd1e97db5, 0x6874d8cb, 0x3471c987, 0x72edb3fc, 0x81c8365d,
		0xe161e33a, 0xfc92db59, 0x2009b1ec, 0xb1a94ce4, 0x2f03696b, 0x87e236d8,
		0x3b2b8ce9, 0x0305e784, 0xf9710883, 0xb039db39, 0x893bea37, 0xe74d6805,
		0x2a5c38bd, 0xb08dc813, 0x15b32375, 0x46be4525, 0x0103fd90, 0xa90e87a2,
		0x52aba66a, 0x85bf7b80, 0x45e8ce63, 0x4dd716d3, 0xf5e30d2d, 0xaf3ae456};
static const u32 g_key3[] = {
		0xa6c8f5ca, 0x6d67c080, 0x924f4d3a, 0x047ca06a, 0x08640297, 0x4fd4a758,
		0xbd685a87, 0x9b2701c2, 0x83b62a35, 0x726b533c, 0xe522fa0c, 0xc24b06b4,
		0x459d1cac, 0xa8c5417b, 0x4fea62a2, 0x0615d742, 0x30628d09, 0xc44fab14,
		0x69ff715e, 0xd2d8837d, 0xbeed0b8b, 0x1e6e57ae, 0x61e8c402, 0xbe367a06,
		0x543f2b5e, 0xdb3ec058, 0xbe852075, 0x1e7e4dcc, 0x1564ea55, 0xec7825b4,
		0xc0538cad, 0x70f72c7f, 0x49e8c3d0, 0xeda97ec5, 0xf492b0a4, 0xe05eb02a};
static const u32 g_key44[] = {
		0xef80e005, 0x3a54689f, 0x43c99ccd, 0x1b7727be, 0x5cb80038, 0xdd2efe62,
		0xf369f92c, 0x160f94c5, 0x29560019, 0xbf3c10c5, 0xf2ce5566, 0xcea2c626,
		0xb601816f, 0x64e7481e, 0x0c34debd, 0x98f29cb0, 0x3fc504d7, 0xc8fb39f0,
		0x0221b3d8, 0x63f936a2, 0x9a3a4800, 0x6ecc32e3, 0x8e120cfd, 0xb0361623,
		0xaee1e689, 0x745502eb, 0xe4a6c61c, 0x74f23eb4, 0xd7fa5813, 0xb01916eb,
		0x12328457, 0xd2bc97d2, 0x646425d8, 0x328380a5, 0x43da8ab1, 0x4b122ac9};
static const u32 g_key20[] = {
		0x33b50800, 0xf32f5fcd, 0x3c14881f, 0x6e8a2a95, 0x29feefd5, 0x1394eae3,
		0xbd6bd443, 0x0821c083, 0xfab379d3, 0xe613e165, 0xf5a754d3, 0x108b2952,
		0x0a4b1e15, 0x61eadeba, 0x557565df, 0x3b465301, 0xae54ecc3, 0x61423309,
		0x70c9ff19, 0x5b0ae5ec, 0x989df126, 0x9d987a5f, 0x55bc750e, 0xc66eba27,
		0x2de988e8, 0xf76600da, 0x0382dccb, 0x5569f5f2, 0x8e431262, 0x288fe3d3,
		0x656f2187, 0x37d12e9c, 0x2f539eb4, 0xa492998e, 0xed3958f7, 0x39e96523};
static const u32 g_key3A[] = {
		0x67877069, 0x3abd5617, 0xc23ab1dc, 0xab57507d, 0x066a7f40, 0x24def9b9,
		0x06f759e4, 0xdcf524b1, 0x13793e5e, 0x0359022d, 0xaae7e1a2, 0x76b9b2fa,
		0x9a160340, 0x87822fba, 0x19e28fbb, 0x9e338a02, 0xd8007e9a, 0xea317af1,
		0x630671de, 0x0b67ca7c, 0x865192af, 0xea3c3526, 0x2b448c8e, 0x8b599254,
		0x4602e9cb, 0x4de16cda, 0xe164d5bb, 0x07ecd88e, 0x99ffe5f8, 0x768800c1,
		0x53b091ed, 0x84047434, 0xb426dbbc, 0x36f948bb, 0x46142158, 0x749bb492};
static const u32 g_keyEBOOT1xx[] = {
		0x18CB69EF, 0x158E8912, 0xDEF90EBB, 0x4CB0FB23, 0x3687EE18, 0x868D4A6E,
		0x19B5C756, 0xEE16551D, 0xE7CB2D6C, 0x9747C660, 0xCE95143F, 0x2956F477,
		0x03824ADE, 0x210C9DF1, 0x5029EB24, 0x81DFE69F, 0x39C89B00, 0xB00C8B91,
		0xEF2DF9C2, 0xE13A93FC, 0x8B94A4A8, 0x491DD09D, 0x686A400D, 0xCED4C7E4,
		0x96C8B7C9, 0x1EAADC28, 0xA4170B84, 0x505D5DDC, 0x5DA6C3CF, 0x0E5DFA2D,
		0x6E7919B5, 0xCE5E29C7, 0xAAACDB94, 0x45F70CDD, 0x62A73725, 0xCCE6563D};
static const u32 g_keyEBOOT2xx[] = {
		0xDA8E36FA, 0x5DD97447, 0x76C19874, 0x97E57EAF, 0x1CAB09BD, 0x9835BAC6,
		0x03D39281, 0x03B205CF, 0x2882E734, 0xE714F663, 0xB96E2775, 0xBD8AAFC7,
		0x1DD3EC29, 0xECA4A16C, 0x5F69EC87, 0x85981E92, 0x7CFCAE21, 0xBAE9DD16,
		0xE6A97804, 0x2EEE02FC, 0x61DF8A3D, 0xDD310564, 0x9697E149, 0xC2453F3B,
		0xF91D8456, 0x39DA6BC8, 0xB3E5FEF5, 0x89C593A3, 0xFB5C8ABC, 0x6C0B7212,
		0xE10DD3CB, 0x98D0B2A8, 0x5FD61847, 0xF0DC2357, 0x7701166A, 0x0F5C3B68};
static const u32 g_keyUPDATER[] = {
		0xA5603CBF, 0xD7482441, 0xF65764CC, 0x1F90060B, 0x4EA73E45, 0xE551D192,
		0xE7B75D8A, 0x465A506E, 0x40FB1022, 0x2C273350, 0x8096DA44, 0x9947198E,
		0x278DEE77, 0x745D062E, 0xC148FA45, 0x832582AF, 0x5FDB86DA, 0xCB15C4CE,
		0x2524C62F, 0x6C2EC3B1, 0x369BE39E, 0xF7EB1FC4, 0x1E51CE1A, 0xD70536F4,
		0xC34D39D8, 0x7418FB13, 0xE3C84DE1, 0xB118F03C, 0xA2018D4E, 0xE6D8770D,
		0x5720F390, 0x17F96341, 0x60A4A68F, 0x1327DD28, 0x05944C64, 0x0C2C4C12};
static const u32 g_keyMEIMG250[] = {
		0xA381FEBC, 0x99B9D5C9, 0x6C560A8D, 0x30309F95, 0x792646CC, 0x82B64E5E,
		0x1A3951AD, 0x0A182EC4, 0xC46131B4, 0x77C50C8A, 0x325F16C6, 0x02D1942E,
		0x0AA38AC4, 0x2A940AC6, 0x67034726, 0xE52DB133, 0xD2EF2107, 0x85C81E90,
		0xC8D164BA, 0xC38DCE1D, 0x948BA275, 0x0DB84603, 0xE2473637, 0xCD74FCDA,
		0x588E3D66, 0x6D28E822, 0x891E548B, 0xF53CF56D, 0x0BBDDB66, 0xC4B286AA,
		0x2BEBBC4B, 0xFC261FF4, 0x92B8E705, 0xDCEE6952, 0x5E0442E5, 0x8BEB7F21};
static const u32 g_keyMEIMG260[] = {
		0x11BFD698, 0xD7F9B324, 0xDD524927, 0x16215B86, 0x504AC36D, 0x5843B217,
		0xE5A0DA47, 0xBB73A1E7, 0x2915DB35, 0x375CFD3A, 0xBB70A905, 0x272BEFCA,
		0x2E960791, 0xEA0799BB, 0xB85AE6C8, 0xC9CAF773, 0x250EE641, 0x06E74A9E,
		0x5244895D, 0x466755A5, 0x9A84AF53, 0xE1024174, 0xEEBA031E, 0xED80B9CE,
		0xBC315F72, 0x5821067F, 0xE8313058, 0xD2D0E706, 0xE6D8933E, 0xD7D17FB4,
		0x505096C4, 0xFDA50B3B, 0x4635AE3D, 0xEB489C8A, 0x422D762D, 0x5A8B3231};
static const u32 g_keyDEMOS27X[] = {
		0x1ABF102F, 0xD596D071, 0x6FC552B2, 0xD4F2531F, 0xF025CDD9, 0xAF9AAF03,
		0xE0CF57CF, 0x255494C4, 0x7003675E, 0x907BC884, 0x002D4EE4, 0x0B687A0D,
		0x9E3AA44F, 0xF58FDA81, 0xEC26AC8C, 0x3AC9B49D, 0x3471C037, 0xB0F3834D,
		0x10DC4411, 0xA232EA31, 0xE2E5FA6B, 0x45594B03, 0xE43A1C87, 0x31DAD9D1,
		0x08CD7003, 0xFA9C2FDF, 0x5A891D25, 0x9B5C1934, 0x22F366E5, 0x5F084A32,
		0x695516D5, 0x2245BE9F, 0x4F6DD705, 0xC4B8B8A1, 0xBC13A600, 0x77B7FC3B};
static const u32 g_keyUNK1[] = {
		0x33B50800, 0xF32F5FCD, 0x3C14881F, 0x6E8A2A95, 0x29FEEFD5, 0x1394EAE3,
		0xBD6BD443, 0x0821C083, 0xFAB379D3, 0xE613E165, 0xF5A754D3, 0x108B2952,
		0x0A4B1E15, 0x61EADEBA, 0x557565DF, 0x3B465301, 0xAE54ECC3, 0x61423309,
		0x70C9FF19, 0x5B0AE5EC, 0x989DF126, 0x9D987A5F, 0x55BC750E, 0xC66EBA27,
		0x2DE988E8, 0xF76600DA, 0x0382DCCB, 0x5569F5F2, 0x8E431262, 0x288FE3D3,
		0x656F2187, 0x37D12E9C, 0x2F539EB4, 0xA492998E, 0xED3958F7, 0x39E96523};
static const u32 g_key_GAMESHARE1xx[] = {
		0x721B53E8, 0xFC3E31C6, 0xF85BA2A2, 0x3CF0AC72, 0x54EEA7AB, 0x5959BFCB,
		0x54B8836B, 0xBC431313, 0x989EF2CF, 0xF0CE36B2, 0x98BA4CF8, 0xE971C931,
		0xA0375DC8, 0x08E52FA0, 0xAC0DD426, 0x57E4D601, 0xC56E61C7, 0xEF1AB98A,
		0xD1D9F8F4, 0x5FE9A708, 0x3EF09D07, 0xFA0C1A8C, 0xA91EEA5C, 0x58F482C5,
		0x2C800302, 0x7EE6F6C3, 0xFF6ABBBB, 0x2110D0D0, 0xD3297A88, 0x980012D3,
		0xDC59C87B, 0x7FDC5792, 0xDB3F5DA6, 0xFC23B787, 0x22698ED3, 0xB680E812};
static const u32 g_key_GAMESHARE2xx[] = {
		0x94A757C7, 0x9FD39833, 0xF8508371, 0x328B0B29, 0x2CBCB9DA, 0x2918B9C6,
		0x944C50BA, 0xF1DCE7D0, 0x640C3966, 0xC90B3D08, 0xF4AD17BA, 0x6CA0F84B,
		0xF7767C67, 0xA4D3A55A, 0x4A085C6A, 0x6BB27071, 0xFA8B38FB, 0x3FDB31B8,
		0x8B7196F2, 0xDB9BED4A, 0x51625B84, 0x4C1481B4, 0xF684F508, 0x30B44770,
		0x93AA8E74, 0x90C579BC, 0x246EC88D, 0x2E051202, 0xC774842E, 0xA185D997,
		0x7A2B3ADD, 0xFE835B6D, 0x508F184D, 0xEB4C4F13, 0x0E1993D3, 0xBA96DFD2};
static const u32 g_key_INDEXDAT1xx[] = {
		0x76CB00AF, 0x111CE62F, 0xB7B27E36, 0x6D8DE8F9, 0xD54BF16A, 0xD9E90373,
		0x7599D982, 0x51F82B0E, 0x636103AD, 0x8E40BC35, 0x2F332C94, 0xF513AAE9,
		0xD22AFEE9, 0x04343987, 0xFC5BB80C, 0x12349D89, 0x14A481BB, 0x25ED3AE8,
		0x7D500E4F, 0x43D1B757, 0x7B59FDAD, 0x4CFBBF34, 0xC3D17436, 0xC1DA21DB,
		0xA34D8C80, 0x962B235D, 0x3E420548, 0x09CF9FFE, 0xD4883F5C, 0xD90E9CB5,
		0x00AEF4E9, 0xF0886DE9, 0x62A58A5B, 0x52A55546, 0x971941B5, 0xF5B79FAC};

struct TAG_INFO
{
	u32 tag; // 4 byte value at offset 0xD0 in the PRX file
	const u32 *key; // "step1_result" use for XOR step
	u8 code;
	u8 codeExtra;
};

static const TAG_INFO g_tagInfo[] =
{
	{ 0x00000000, g_key0, 0x42 },
	{ 0x02000000, g_key2, 0x45 },
	{ 0x03000000, g_key3, 0x46 },
	{ 0x4467415d, g_key44, 0x59, 0x59 },
	{ 0x207bbf2f, g_key20, 0x5A, 0x5A },
	{ 0x3ace4dce, g_key3A, 0x5B, 0x5B },
	{ 0x07000000, g_key_INDEXDAT1xx, 0x4A },
	{ 0x08000000, g_keyEBOOT1xx, 0x4B },
	{ 0xC0CB167C, g_keyEBOOT2xx, 0x5D, 0x5D },
	{ 0x0B000000, g_keyUPDATER, 0x4E },
	{ 0x0C000000, g_keyDEMOS27X, 0x4F },
	{ 0x0F000000, g_keyMEIMG250, 0x52 },
	{ 0x862648D1, g_keyMEIMG260, 0x52, 0x52 },
	{ 0x207BBF2F, g_keyUNK1, 0x5A, 0x5A },
	{ 0x09000000, g_key_GAMESHARE1xx, 0x4C },
	{ 0xBB67C59F, g_key_GAMESHARE2xx, 0x5E, 0x5E }
};

bool HasKey(int key)
{
	switch (key)
	{
		case 0x02: case 0x03: case 0x04: case 0x05: case 0x07: case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12:
		case 0x38: case 0x39: case 0x3A: case 0x44: case 0x4B:
		case 0x53: case 0x57: case 0x5D:
		case 0x63: case 0x64:
			return true;
		default:
			INFO_LOG(HLE, "Missing key %02X, cannot decrypt module", key);
			return false;
	}
}

static const TAG_INFO *GetTagInfo(u32 tagFind)
{
	for (u32 iTag = 0; iTag < sizeof(g_tagInfo)/sizeof(TAG_INFO); iTag++)
		if (g_tagInfo[iTag].tag == tagFind)
			return &g_tagInfo[iTag];
	return NULL; // not found
}

static void ExtraV2Mangle(u8* buffer1, u8 codeExtra)
{
	u8 buffer2[ROUNDUP16(0x14+0xA0)];

	memcpy(buffer2+0x14, buffer1, 0xA0);

	u32* pl2 = (u32*)buffer2;
	pl2[0] = 5;
	pl2[1] = pl2[2] = 0;
	pl2[3] = codeExtra;
	pl2[4] = 0xA0;

	sceUtilsBufferCopyWithRange(buffer2, 20+0xA0, buffer2, 20+0xA0, KIRK_CMD_DECRYPT_IV_0);
	// copy result back
	memcpy(buffer1, buffer2, 0xA0);
}

static int Scramble(u32 *buf, u32 size, u32 code)
{
	buf[0] = 5;
	buf[1] = buf[2] = 0;
	buf[3] = code;
	buf[4] = size;

	if (sceUtilsBufferCopyWithRange((u8*)buf, size+0x14, (u8*)buf, size+0x14, KIRK_CMD_DECRYPT_IV_0) < 0)
	{
		return -1;
	}

	return 0;
}

static int DecryptPRX1(const u8* pbIn, u8* pbOut, int cbTotal, u32 tag)
{
	int i, retsize;
	u8 bD0[0x80], b80[0x50], b00[0x80], bB0[0x20];

	const TAG_INFO *pti = GetTagInfo(tag);
	if (pti == NULL)
	{
		return -1;
	}
	if (!HasKey(pti->code) || 
		(pti->codeExtra != 0 && !HasKey(pti->codeExtra)))
	{
		return MISSING_KEY;
	}

	retsize = *(u32*)&pbIn[0xB0];

	for (i = 0; i < 0x14; i++)
	{
		if (pti->key[i] != 0)
			break;
	}

	// Scramble the key (!)
	//
	// NOTE: I can't make much sense out of this code. Scramble seems really odd, appears
	// to write to stuff that should be before the actual key.
	u8 key[0x90];
	memcpy(key, pti->key, 0x90);
	if (i == 0x14)
	{
		Scramble((u32 *)key, 0x90, pti->code);
	}

	// build conversion into pbOut

	if (pbIn != pbOut)
		memcpy(pbOut, pbIn, cbTotal);

	memcpy(bD0, pbIn+0xD0, 0x80);
	memcpy(b80, pbIn+0x80, 0x50);
	memcpy(b00, pbIn+0x00, 0x80);
	memcpy(bB0, pbIn+0xB0, 0x20);

	memset(pbOut, 0, 0x150);
	memset(pbOut, 0x55, 0x40); // first $40 bytes ignored

	// step3 demangle in place
	u32* pl = (u32*)(pbOut+0x2C);
	pl[0] = 5; // number of ulongs in the header
	pl[1] = pl[2] = 0;
	pl[3] = pti->code; // initial seed for PRX
	pl[4] = 0x70;   // size

	// redo part of the SIG check (step2)
	u8 buffer1[0x150];
	memcpy(buffer1+0x00, bD0, 0x80);
	memcpy(buffer1+0x80, b80, 0x50);
	memcpy(buffer1+0xD0, b00, 0x80);
	if (pti->codeExtra != 0)
		ExtraV2Mangle(buffer1+0x10, pti->codeExtra);
	memcpy(pbOut+0x40, buffer1+0x40, 0x40);

	int ret;
	int iXOR;
	for (iXOR = 0; iXOR < 0x70; iXOR++)
		pbOut[0x40+iXOR] = pbOut[0x40+iXOR] ^ key[0x14+iXOR];

	ret = sceUtilsBufferCopyWithRange(pbOut+0x2C, 20+0x70, pbOut+0x2C, 20+0x70, 7);
	if (ret != 0)
	{
		return -1;
	}

	for (iXOR = 0x6F; iXOR >= 0; iXOR--)
		pbOut[0x40+iXOR] = pbOut[0x2C+iXOR] ^ key[0x20+iXOR];

	memset(pbOut+0x80, 0, 0x30); // $40 bytes kept, clean up
	pbOut[0xA0] = 1;
	// copy unscrambled parts from header
	memcpy(pbOut+0xB0, bB0, 0x20); // file size + lots of zeros
	memcpy(pbOut+0xD0, b00, 0x80); // ~PSP header

	// step4: do the actual decryption of code block
	//  point 0x40 bytes into the buffer to key info
	ret = sceUtilsBufferCopyWithRange(pbOut, cbTotal, pbOut+0x40, cbTotal-0x40, 0x1);
	if (ret != 0)
	{
		return -1;
	}

	// return cbTotal - 0x150; // rounded up size
	return retsize;
}

////////// Decryption 2 //////////

struct TAG_INFO2
{
	u32 tag; // 4 byte value at offset 0xD0 in the PRX file
	const u8 *key; // 16 bytes keys
	u8 code; // code for scramble
	u8 type;
};

static const TAG_INFO2 g_tagInfo2[] =
{
	{ 0x4C9494F0, keys660_k1, 0x43 },
	{ 0x4C9495F0, keys660_k2, 0x43 },
	{ 0x4C9490F0, keys660_k3, 0x43 },
	{ 0x4C9491F0, keys660_k8, 0x43 },
	{ 0x4C9493F0, keys660_k4, 0x43 },
	{ 0x4C9497F0, keys660_k5, 0x43 },
	{ 0x4C9492F0, keys660_k6, 0x43 },
	{ 0x4C9496F0, keys660_k7, 0x43 },
	{ 0x457B90F0, keys660_v1, 0x5B },
	{ 0x457B91F0, keys660_v7, 0x5B },
	{ 0x457B92F0, keys660_v6, 0x5B },
	{ 0x457B93F0, keys660_v3, 0x5B },
	{ 0x380290F0, keys660_v2, 0x5A },
	{ 0x380291F0, keys660_v8, 0x5A },
	{ 0x380292F0, keys660_v4, 0x5A },
	{ 0x380293F0, keys660_v5, 0x5A },
	{ 0x4C948CF0, keys639_k3, 0x43 },
	{ 0x4C948DF0, keys638_k4, 0x43 },
	{ 0x4C948BF0, keys636_k2, 0x43 },
	{ 0x4C948AF0, keys636_k1, 0x43 },
	{ 0x4C9487F0, keys630_k8, 0x43 },
	{ 0x457B83F0, keys630_k7, 0x5B },
	{ 0x4C9486F0, keys630_k6, 0x43 },
	{ 0x457B82F0, keys630_k5, 0x5B },
	{ 0x457B81F0, keys630_k4, 0x5B },
	{ 0x4C9485F0, keys630_k3, 0x43 },
	{ 0x457B80F0, keys630_k2, 0x5B },
	{ 0x4C9484F0, keys630_k1, 0x43 },
	{ 0x457B28F0, keys620_e, 0x5B },
	{ 0x457B0CF0, keys620_a, 0x5B },
	{ 0x380228F0, keys620_5v, 0x5A },
	{ 0x4C942AF0, keys620_5k, 0x43 },
	{ 0x4C9428F0, keys620_5, 0x43 },
	{ 0x4C941DF0, keys620_1, 0x43 },
	{ 0x4C941CF0, keys620_0, 0x43 },
	{ 0x4C9422F0, keys600_2, 0x43 },
	{ 0x4C941EF0, keys600_1, 0x43 },
	{ 0x4C9429F0, keys570_5k, 0x43 },
	{ 0x457B0BF0, keys505_a, 0x5B },
	{ 0x4C9419F0, keys505_1, 0x43 },
	{ 0x4C9418F0, keys505_0, 0x43 },
	{ 0x457B1EF0, keys500_c, 0x5B },
	{ 0x4C941FF0, keys500_2, 0x43 },
	{ 0x4C9417F0, keys500_1, 0x43 },
	{ 0x4C9416F0, keys500_0, 0x43 },
	{ 0x4C9414F0, keys390_0, 0x43 },
	{ 0x4C9415F0, keys390_1, 0x43 },
	{ 0x4C9412F0, keys370_0, 0x43 },
	{ 0x4C9413F0, keys370_1, 0x43 },
	{ 0x457B10F0, keys370_2, 0x5B },
	{ 0x4C940DF0, keys360_0, 0x43 },
	{ 0x4C9410F0, keys360_1, 0x43 },
	{ 0x4C940BF0, keys330_0, 0x43 },
	{ 0x457B0AF0, keys330_1, 0x5B },
	{ 0x38020AF0, keys330_2, 0x5A },
	{ 0x4C940AF0, keys330_3, 0x43 },
	{ 0x4C940CF0, keys330_4, 0x43 },
	{ 0xcfef09f0, keys310_0, 0x62 },
	{ 0x457b08f0, keys310_1, 0x5B },
	{ 0x380208F0, keys310_2, 0x5A },
	{ 0xcfef08f0, keys310_3, 0x62 },
	{ 0xCFEF07F0, keys303_0, 0x62 },
	{ 0xCFEF06F0, keys300_0, 0x62 },
	{ 0x457B06F0, keys300_1, 0x5B },
	{ 0x380206F0, keys300_2, 0x5A },
	{ 0xCFEF05F0, keys280_0, 0x62 },
	{ 0x457B05F0, keys280_1, 0x5B },
	{ 0x380205F0, keys280_2, 0x5A },
	{ 0x16D59E03, keys260_0, 0x62 },
	{ 0x76202403, keys260_1, 0x5B },
	{ 0x0F037303, keys260_2, 0x5A },
	{ 0x4C940FF0, key_2DA8, 0x43 },
	{ 0x4467415D, key_22E0, 0x59 },
	{ 0x00000000, key_21C0, 0x42 },
	{ 0x01000000, key_2250, 0x43 },
	{ 0x2E5E10F0, key_2E5E10F0, 0x48 },
	{ 0x2E5E12F0, key_2E5E12F0, 0x48 },
	{ 0x2E5E13F0, key_2E5E13F0, 0x48 },
	{ 0x2FD30BF0, key_2FD30BF0, 0x47 },
	{ 0x2FD311F0, key_2FD311F0, 0x47 },
	{ 0x2FD312F0, key_2FD312F0, 0x47 },
	{ 0xD91605F0, key_D91605F0, 0x5D },
	{ 0xD91606F0, key_D91606F0, 0x5D },
	{ 0xD91608F0, key_D91608F0, 0x5D },
	{ 0xD91609F0, key_D91609F0, 0x5D },
	{ 0xD9160AF0, key_D9160AF0, 0x5D },
	{ 0xD9160BF0, key_D9160BF0, 0x5D },
	{ 0xD91611F0, key_D91611F0, 0x5D },
	{ 0xD91612F0, key_D91612F0, 0x5D },
	{ 0xD91613F0, key_D91613F0, 0x5D },
	{ 0xD91614F0, key_D91614F0, 0x5D },
	{ 0xD91615F0, key_D91615F0, 0x5D },
	{ 0xD91616F0, key_D91616F0, 0x5D },
	{ 0xD91617F0, key_D91617F0, 0x5D },
	{ 0xD91618F0, key_D91618F0, 0x5D },
	{ 0xD91619F0, key_D91619F0, 0x5D },
	{ 0xD9161AF0, key_D9161AF0, 0x5D },
	{ 0xD91620F0, key_D91620F0, 0x5D },
	{ 0xD91621F0, key_D91621F0, 0x5D },
	{ 0xD91622F0, key_D91622F0, 0x5D },
	{ 0xD91623F0, key_D91623F0, 0x5D },
	{ 0xD91624F0, key_D91624F0, 0x5D },
	{ 0xD91628F0, key_D91628F0, 0x5D },
	{ 0xD91680F0, key_D91680F0, 0x5D },
	{ 0xD91681F0, key_D91681F0, 0x5D },
	{ 0xD82310F0, keys02G_E, 0x51 },
	{ 0xD8231EF0, keys03G_E, 0x51 },
	{ 0xD82328F0, keys05G_E, 0x51 },
	{ 0x279D08F0, oneseg_310, 0x61 },
	{ 0x279D06F0, oneseg_300, 0x61 },
	{ 0x279D05F0, oneseg_280, 0x61 },
	{ 0xD66DF703, oneseg_260_271, 0x61 },
	{ 0x279D10F0, oneseg_slim, 0x61 },
	{ 0x3C2A08F0, ms_app_main, 0x67 },
	{ 0xADF305F0, demokeys_280, 0x60 },
	{ 0xADF306F0, demokeys_3XX_1, 0x60 },
	{ 0xADF308F0, demokeys_3XX_2, 0x60 },
	{ 0x8004FD03, ebootbin_271_new, 0x5D },
	{ 0xD91605F0, ebootbin_280_new, 0x5D },
	{ 0xD91606F0, ebootbin_300_new, 0x5D },
	{ 0xD91608F0, ebootbin_310_new, 0x5D },
	{ 0x0A35EA03, gameshare_260_271, 0x5E },
	{ 0x7B0505F0, gameshare_280, 0x5E },
	{ 0x7B0506F0, gameshare_300, 0x5E },
	{ 0x7B0508F0, gameshare_310, 0x5E },
	{ 0x380210F0, key_380210F0, 0x5A },
	{ 0x380280F0, key_380280F0, 0x5A },
	{ 0x380283F0, key_380283F0, 0x5A },
	{ 0x407810F0, key_407810F0, 0x6A },
	{ 0xE92410F0, drmkeys_6XX_1, 0x40 },
	{ 0x692810F0, drmkeys_6XX_2, 0x40 }
};


static const TAG_INFO2 *GetTagInfo2(u32 tagFind)
{
	for (u32 iTag = 0; iTag < sizeof(g_tagInfo2) / sizeof(TAG_INFO2); iTag++)
	{
		if (g_tagInfo2[iTag].tag == tagFind)
		{
			return &g_tagInfo2[iTag];
		}
	}

	return NULL; // not found
}


static int DecryptPRX2(const u8 *inbuf, u8 *outbuf, u32 size, u32 tag)
{
	const TAG_INFO2 *pti = GetTagInfo2(tag);

	if (!pti)
	{
		return -1;
	}
	if (!HasKey(pti->code))
	{
		return MISSING_KEY;
	}

	int retsize = *(const int *)&inbuf[0xB0];
	u8 tmp1[0x150] = {0};
	u8 tmp2[ROUNDUP16(0x90+0x14)] = {0};
	u8 tmp3[ROUNDUP16(0x90+0x14)] = {0};
	u8 tmp4[ROUNDUP16(0x20)] = {0};

	if (inbuf != outbuf)
		memcpy(outbuf, inbuf, size);

	if (size < 0x160)
	{
		return -2;
	}

	if (((int)size - 0x150) < retsize)
	{
		return -4;
	}

	memcpy(tmp1, outbuf, 0x150);

	int i, j;
	u8 *p = tmp2 + 0x14;

	// Writes 0x90 bytes to tmp2 + 0x14.
	for (i = 0; i < 9; i++)
	{
		for (j = 0; j < 0x10; j++)
		{
			p[(i << 4) + j] = pti->key[j];
		}
		p[(i << 4)] = i;   // really? this is very odd
	}

	if (Scramble((u32 *)tmp2, 0x90, pti->code) < 0)
	{
		return -5;
	}

	memcpy(outbuf, tmp1+0xD0, 0x5C);
	memcpy(outbuf+0x5C, tmp1+0x140, 0x10);
	memcpy(outbuf+0x6C, tmp1+0x12C, 0x14);
	memcpy(outbuf+0x80, tmp1+0x080, 0x30);
	memcpy(outbuf+0xB0, tmp1+0x0C0, 0x10);
	memcpy(outbuf+0xC0, tmp1+0x0B0, 0x10);
	memcpy(outbuf+0xD0, tmp1+0x000, 0x80);

	memcpy(tmp3+0x14, outbuf+0x5C, 0x60);

	if (Scramble((u32 *)tmp3, 0x60, pti->code) < 0)
	{
		return -6;
	}

	memcpy(outbuf+0x5C, tmp3, 0x60);
	memcpy(tmp3, outbuf+0x6C, 0x14);
	memcpy(outbuf+0x70, outbuf+0x5C, 0x10);

	if(pti->type == 3)
	{
		memcpy(tmp4, outbuf+0x3C, 0x20);
		memcpy(outbuf+0x50, tmp4, 0x20);
		memset(outbuf+0x18, 0, 0x38);
	}else
		memset(outbuf+0x18, 0, 0x58);

	memcpy(outbuf+0x04, outbuf, 0x04);
	*((u32 *)outbuf) = 0x014C;
	memcpy(outbuf+0x08, tmp2, 0x10);

	/* sha-1 */

	if (sceUtilsBufferCopyWithRange(outbuf, 3000000, outbuf, 3000000, 0x0B) != 0)
	{
		return -7;
	}

	if (memcmp(outbuf, tmp3, 0x14) != 0)
	{
		return -8;
	}

	for (int iXOR = 0; iXOR < 0x40; iXOR++)
	{
		tmp3[iXOR+0x14] = outbuf[iXOR+0x80] ^ tmp2[iXOR+0x10];
	}

	if (Scramble((u32 *)tmp3, 0x40, pti->code) != 0)
	{
		return -9;
	}

	for (int iXOR = 0x3F; iXOR >= 0; iXOR--)
	{
		outbuf[iXOR+0x40] = tmp3[iXOR] ^ tmp2[iXOR+0x50]; // uns 8
	}

	if (pti->type == 3)
	{
		memcpy(outbuf+0x80, tmp4, 0x20);
		memset(outbuf+0xA0, 0, 0x10);
		*(u32*)&outbuf[0xA4] = 1;
		*(u32*)&outbuf[0xA0] = 1;
	}
	else
	{
		memset(outbuf+0x80, 0, 0x30);
		*(u32*)&outbuf[0xA0] = 1;
	}

	memcpy(outbuf+0xB0, outbuf+0xC0, 0x10);
	memset(outbuf+0xC0, 0, 0x10);
	memcpy(outbuf+0xD0, outbuf+0xD0, 0x80);

	// The real decryption
	if (sceUtilsBufferCopyWithRange(outbuf, size, outbuf + 0x40, size - 0x40, 0x1) != 0)
	{
		return -1;
	}

	if (retsize < 0x150)
	{
		// Fill with 0
		memset(outbuf+retsize, 0, 0x150-retsize);
	}

	return retsize;
}

int pspDecryptPRX(const u8 *inbuf, u8 *outbuf, u32 size)
{
	kirk_init();
	int retsize = DecryptPRX1(inbuf, outbuf, size, *(u32 *)&inbuf[0xD0]);
	if (retsize == MISSING_KEY)
	{
		return MISSING_KEY;
	}

	if (retsize <= 0)
	{
		retsize = DecryptPRX2(inbuf, outbuf, size, *(u32 *)&inbuf[0xD0]);
	}

	return retsize;
}

