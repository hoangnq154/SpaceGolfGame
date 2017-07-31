#include "GuiBgfxTexture.h"
#include <bgfx/bgfx.h>
#include <bgfx_utils.h>

using namespace CEGUI;

GuiBgfxTexture::GuiBgfxTexture(String name)
{
	this->name = name;
}


GuiBgfxTexture::~GuiBgfxTexture()
{
}

const String & GuiBgfxTexture::getName() const
{
	return name;
}

const Sizef & GuiBgfxTexture::getSize() const
{
	return size;
}

const Sizef & GuiBgfxTexture::getOriginalDataSize() const
{
	return size;
}

const Vector2f & GuiBgfxTexture::getTexelScaling() const
{
	return Vector2f(1, 1);
}

void GuiBgfxTexture::loadFromFile(const String & filename, const String & resourceGroup)
{
	RawDataContainer texFile;
	System::getSingleton().getResourceProvider()->
		loadRawDataContainer(filename, texFile, resourceGroup);

	// get and check existence of CEGUI::System (needed to access ImageCodec)
	System* sys = System::getSingletonPtr();
	if (!sys)
		CEGUI_THROW(RendererException(
			"CEGUI::System object has not been created: "
			"unable to access ImageCodec."));

	Texture* res = sys->getImageCodec().load(texFile, this);

	// unload file data buffer
	System::getSingleton().getResourceProvider()->
		unloadRawDataContainer(texFile);

	if (!res)
		// It's an error
		CEGUI_THROW(RendererException(
			sys->getImageCodec().getIdentifierString() +
			" failed to load image '" + filename + "'."));
}

void GuiBgfxTexture::loadFromMemory(const void * buffer, const Sizef & buffer_size, PixelFormat pixel_format)
{
	size = buffer_size;
	long bytes;
	bgfx::TextureFormat::Enum format;
	switch (pixel_format) {
	case PixelFormat::PF_RGB:
		format = bgfx::TextureFormat::RGB8;
		bytes = 3;
		break;
	case PixelFormat::PF_RGBA:
		format = bgfx::TextureFormat::RGBA8;
		bytes = 4;
		break;
	default:
		CEGUI_THROW(RendererException("Unsupported pixel format"));
	}
	auto mem = bgfx::makeRef(buffer, buffer_size.d_width * buffer_size.d_height * bytes);
	loadFromMemory(mem, buffer_size, format);
}

void GuiBgfxTexture::loadFromMemory(const bgfx::Memory* mem, const Sizef & buffer_size, bgfx::TextureFormat::Enum format)
{
	handle = bgfx::createTexture2D(
		uint16_t(buffer_size.d_width)
		, uint16_t(buffer_size.d_height)
		, 1
		, 1
		, format
		, 0
		, mem
	);
}

void GuiBgfxTexture::blitFromMemory(const void * sourceData, const Rectf & area)
{
	bgfx::updateTexture2D(handle, 0, 0,
		area.d_min.d_x, area.d_min.d_y,
		area.d_max.d_x - area.d_min.d_x, area.d_max.d_y - area.d_min.d_y,
		bgfx::makeRef(sourceData, size.d_width * size.d_height));
}

void GuiBgfxTexture::blitToMemory(void * targetData)
{
	bgfx::readTexture(handle, targetData);
}

bool GuiBgfxTexture::isPixelFormatSupported(const PixelFormat fmt) const
{
	return true;
}

void GuiBgfxTexture::destroy()
{
	bgfx::destroyTexture(handle);
}