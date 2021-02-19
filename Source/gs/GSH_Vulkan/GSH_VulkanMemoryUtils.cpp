#include "GSH_VulkanMemoryUtils.h"

using namespace GSH_Vulkan;

Nuanceur::CIntRvalue CMemoryUtils::GetPixelAddress_PSMT4(Nuanceur::CShaderBuilder& b, Nuanceur::CImageUint2DValue swizzleTable,
                                                         Nuanceur::CIntValue bufAddress, Nuanceur::CIntValue bufWidth, Nuanceur::CInt2Value position)
{
	auto pageWidth = NewInt(b, CGsPixelFormats::STORAGEPSMT4::PAGEWIDTH);
	auto pageHeight = NewInt(b, CGsPixelFormats::STORAGEPSMT4::PAGEHEIGHT);
	auto pageSize = NewInt(b, CGsPixelFormats::PAGESIZE);
	auto pageNum = (position->x() / pageWidth) + ((position->y() / pageHeight) * bufWidth) / pageWidth;
	auto pagePos = NewInt2(position->x() % pageWidth, position->y() % pageHeight);
	auto pageOffset = ToInt(Load(swizzleTable, pagePos)->x());
	return ((bufAddress + (pageNum * pageSize)) * NewInt(b, 2)) + pageOffset;
}

Nuanceur::CUintRvalue CMemoryUtils::Memory_Read32(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address)
{
	auto wordAddress = address / NewInt(b, 4);
	return Load(memoryBuffer, wordAddress);
}

Nuanceur::CUintRvalue CMemoryUtils::Memory_Read24(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address)
{
	auto wordAddress = address / NewInt(b, 4);
	return Load(memoryBuffer, wordAddress) & NewUint(b, 0xFFFFFF);
}

Nuanceur::CUintRvalue CMemoryUtils::Memory_Read16(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address)
{
	auto wordAddress = address / NewInt(b, 4);
	auto shiftAmount = (ToUint(address) & NewUint(b, 2)) * NewUint(b, 8);
	auto pixel = Load(memoryBuffer, wordAddress);
	return (pixel >> shiftAmount) & NewUint(b, 0xFFFF);
}

Nuanceur::CUintRvalue CMemoryUtils::Memory_Read8(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address)
{
	auto wordAddress = address / NewInt(b, 4);
	auto shiftAmount = (ToUint(address) & NewUint(b, 3)) * NewUint(b, 8);
	auto pixel = Load(memoryBuffer, wordAddress);
	return (pixel >> shiftAmount) & NewUint(b, 0xFF);
}

Nuanceur::CUintRvalue CMemoryUtils::Memory_Read4(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue nibAddress)
{
	auto wordAddress = nibAddress / NewInt(b, 8);
	auto shiftAmount = (ToUint(nibAddress) & NewUint(b, 7)) * NewUint(b, 4);
	auto pixel = Load(memoryBuffer, wordAddress);
	return (pixel >> shiftAmount) & NewUint(b, 0xF);
}

void CMemoryUtils::Memory_Write32(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress = address / NewInt(b, 4);
	Store(memoryBuffer, wordAddress, value, 32);
}

void CMemoryUtils::Memory_Write24(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress = address / NewInt(b, 4);
	auto mask = NewUint(b, 0xFF000000);
	#if 0
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, value);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | value;
		Store(memoryBuffer, wordAddress, pixel, 24);
	#elif 0
		Store(memoryBuffer, address, value, 8);
		Store(memoryBuffer, address + NewInt(b, 1), value >> NewUint(b, 8), 8);
		Store(memoryBuffer, address + NewInt(b, 2), value >> NewUint(b, 16), 8);
	#else
		Store(memoryBuffer, address, value, 16);
		Store(memoryBuffer, address + NewInt(b, 2), value >> NewUint(b, 16), 8);
	#endif
}

void CMemoryUtils::Memory_Write24_orig(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress = address / NewInt(b, 4);
	auto mask = NewUint(b, 0xFF000000);
	#if 1
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, value);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | value;
		Store(memoryBuffer, wordAddress, pixel, 24);
	#elif 1
		Store(memoryBuffer, address, value, 8);
		Store(memoryBuffer, address + NewInt(b, 1), value >> NewUint(b, 8), 8);
		Store(memoryBuffer, address + NewInt(b, 2), value >> NewUint(b, 16), 8);
	#else
		Store(memoryBuffer, address, value, 16);
		Store(memoryBuffer, address + NewInt(b, 2), value >> NewUint(b, 16), 8);
	#endif
}

void CMemoryUtils::Memory_Write16(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress =  (address +  NewInt(b, 0)) / NewInt(b, 4);
	auto shiftAmount = (ToUint(address) & NewUint(b, 2)) * NewUint(b, 8);
	auto mask = NewUint(b, 0xFFFFFFFF) ^ (NewUint(b, 0xFFFF) << shiftAmount);
	auto valueWord = value << shiftAmount;
	#if 0
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, valueWord);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | valueWord;
		Store(memoryBuffer, address, pixel, 32);
	#else
		#if 0
			Store(memoryBuffer, address + NewInt(b, 1), value, 8);
			Store(memoryBuffer, address, value >> NewUint(b, 8), 8);
		#elif 0
			Store(memoryBuffer, address, value, 8);
			Store(memoryBuffer, address + NewInt(b, 1), value >> NewUint(b, 8), 8);
		#else
			// auto val = (value >> NewUint(b, 8)) | (value << NewUint(b, 8));

			Store(memoryBuffer, address, value, 16);
		#endif
		
	#endif
}

void CMemoryUtils::Memory_Write16_orig(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress =  (address +  NewInt(b, 0)) / NewInt(b, 4);
	auto shiftAmount = (ToUint(address) & NewUint(b, 2)) * NewUint(b, 8);
	auto mask = NewUint(b, 0xFFFFFFFF) ^ (NewUint(b, 0xFFFF) << shiftAmount);
	auto valueWord = value << shiftAmount;
	#if 1
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, valueWord);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | valueWord;
		Store(memoryBuffer, address, pixel, 32);
	#else
		#if 0
			Store(memoryBuffer, address + NewInt(b, 1), value, 8);
			Store(memoryBuffer, address, value >> NewUint(b, 8), 8);
		#elif 1
			Store(memoryBuffer, address, value, 8);
			Store(memoryBuffer, address + NewInt(b, 1), value >> NewUint(b, 8), 8);
		#else
			// auto val = (value >> NewUint(b, 8)) | (value << NewUint(b, 8));

			Store(memoryBuffer, address, value, 16);
		#endif
		
	#endif
}

void CMemoryUtils::Memory_Write8(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue address, Nuanceur::CUintValue value)
{
	auto wordAddress = address / NewInt(b, 4);
	auto shiftAmount = (ToUint(address) & NewUint(b, 3)) * NewUint(b, 8);
	auto mask = NewUint(b, 0xFFFFFFFF) ^ (NewUint(b, 0xFF) << shiftAmount);
	auto valueWord = value << shiftAmount;
	#if 0
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, valueWord);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | valueWord;
		Store(memoryBuffer, wordAddress, pixel, 32);
	#else
		Store(memoryBuffer, address, value, 8);
	#endif
}

void CMemoryUtils::Memory_Write4(Nuanceur::CShaderBuilder& b, Nuanceur::CArrayUintValue memoryBuffer, Nuanceur::CIntValue nibAddress, Nuanceur::CUintValue value)
{
	auto wordAddress = nibAddress / NewInt(b, 8);
	auto shiftAmount = (ToUint(nibAddress) & NewUint(b, 7)) * NewUint(b, 4);
	auto mask = NewUint(b, 0xFFFFFFFF) ^ (NewUint(b, 0xF) << shiftAmount);
	auto valueWord = value << shiftAmount;
	#if 1
		// return;
		AtomicAnd(memoryBuffer, wordAddress, mask);
		AtomicOr(memoryBuffer, wordAddress, valueWord);
	#elif 0
		auto pixel = (Load(memoryBuffer, wordAddress) & mask) | valueWord;
		Store(memoryBuffer, wordAddress, pixel, 32);
	#else
		Store(memoryBuffer, NewInt(b, 2), value, 4);
	#endif
}

Nuanceur::CFloat4Rvalue CMemoryUtils::PSM32ToVec4(Nuanceur::CShaderBuilder& b, Nuanceur::CUintValue inputColor)
{
	auto colorR = (inputColor >> NewUint(b, 0)) & NewUint(b, 0xFF);
	auto colorG = (inputColor >> NewUint(b, 8)) & NewUint(b, 0xFF);
	auto colorB = (inputColor >> NewUint(b, 16)) & NewUint(b, 0xFF);
	auto colorA = (inputColor >> NewUint(b, 24)) & NewUint(b, 0xFF);

	auto colorFloatR = ToFloat(colorR) / NewFloat(b, 255.f);
	auto colorFloatG = ToFloat(colorG) / NewFloat(b, 255.f);
	auto colorFloatB = ToFloat(colorB) / NewFloat(b, 255.f);
	auto colorFloatA = ToFloat(colorA) / NewFloat(b, 255.f);

	return NewFloat4(colorFloatR, colorFloatG, colorFloatB, colorFloatA);
}

Nuanceur::CFloat4Rvalue CMemoryUtils::PSM16ToVec4(Nuanceur::CShaderBuilder& b, Nuanceur::CUintValue inputColor)
{
	auto colorR = (inputColor >> NewUint(b, 0)) & NewUint(b, 0x1F);
	auto colorG = (inputColor >> NewUint(b, 5)) & NewUint(b, 0x1F);
	auto colorB = (inputColor >> NewUint(b, 10)) & NewUint(b, 0x1F);
	auto colorA = (inputColor >> NewUint(b, 15)) & NewUint(b, 0x1);

	auto colorFloatR = ToFloat(colorR) / NewFloat(b, 31.f);
	auto colorFloatG = ToFloat(colorG) / NewFloat(b, 31.f);
	auto colorFloatB = ToFloat(colorB) / NewFloat(b, 31.f);
	auto colorFloatA = ToFloat(colorA);

	return NewFloat4(colorFloatR, colorFloatG, colorFloatB, colorFloatA);
}

Nuanceur::CUintRvalue CMemoryUtils::Vec4ToPSM32(Nuanceur::CShaderBuilder& b, Nuanceur::CFloat4Value inputColor)
{
	auto colorR = ToUint(inputColor->x() * NewFloat(b, 255.f)) << NewUint(b, 0);
	auto colorG = ToUint(inputColor->y() * NewFloat(b, 255.f)) << NewUint(b, 8);
	auto colorB = ToUint(inputColor->z() * NewFloat(b, 255.f)) << NewUint(b, 16);
	auto colorA = ToUint(inputColor->w() * NewFloat(b, 255.f)) << NewUint(b, 24);
	return colorR | colorG | colorB | colorA;
}

Nuanceur::CUintRvalue CMemoryUtils::Vec4ToPSM16(Nuanceur::CShaderBuilder& b, Nuanceur::CFloat4Value inputColor)
{
	auto colorR = ToUint(inputColor->x() * NewFloat(b, 31.f)) << NewUint(b, 0);
	auto colorG = ToUint(inputColor->y() * NewFloat(b, 31.f)) << NewUint(b, 5);
	auto colorB = ToUint(inputColor->z() * NewFloat(b, 31.f)) << NewUint(b, 10);
	auto colorA = ToUint(inputColor->w()) << NewUint(b, 15);
	return colorR | colorG | colorB | colorA;
}
