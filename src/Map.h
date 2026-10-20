#pragma once

#include "Module.h"
#include "Vector2D.h"
#include <SDL3/SDL_rect.h>
#include <string>
#include <vector>
#include <cstdlib>

struct SDL_Texture;

// Add name/value to Property, and the typed accessors below --
// value is a string so a property can hold a bool, an int, a float or text.
struct Properties
{
	struct Property
	{
		std::string name;
		std::string value;

		bool AsBool(bool defaultValue = false) const
		{
			return value.empty() ? defaultValue : (value == "true" || value == "1");
		}

		int AsInt(int defaultValue = 0) const
		{
			return value.empty() ? defaultValue : std::atoi(value.c_str());
		}

		float AsFloat(float defaultValue = 0.0f) const
		{
			return value.empty() ? defaultValue : (float)std::atof(value.c_str());
		}

		const std::string& AsString() const { return value; }
	};

	// The properties belonging to a single map or layer node
	std::vector<Property> list;

	// Return the property with this name, or nullptr if none
	const Property* GetProperty(const std::string& name) const
	{
		for (const auto& property : list) {
			if (property.name == name) return &property;
		}
		return nullptr;
	}
};

// Create a struct to hold information for a single map layer
struct MapLayer
{
	int id;
	std::string name;
	int width;
	int height;
	bool visible;   // native TMX attribute; hides helper layers like the collision mask
	std::vector<int> tiles;

	// Custom properties read from this layer's <properties> node
	Properties properties;

	// Return the gid at tile (i, j)
	unsigned int Get(int i, int j) const
	{
		return tiles[(j * width) + i];
	}
};

// Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
	int firstGid;
	std::string name;
	int tileWidth;
	int tileHeight;
	int spacing;
	int margin;
	int tileCount;
	int columns;
	SDL_Texture* texture;

	// Return the source rect for this gid within the tileset image
	SDL_Rect GetRect(unsigned int gid) const
	{
		SDL_Rect rect = { 0 };

		int relativeIndex = gid - firstGid;
		rect.w = tileWidth;
		rect.h = tileHeight;
		rect.x = margin + (tileWidth + spacing) * (relativeIndex % columns);
		rect.y = margin + (tileHeight + spacing) * (relativeIndex / columns);

		return rect;
	}
};

// Create a struct needed to hold the information of the <map> node
struct MapData
{
	int width;
	int height;
	int tileWidth;
	int tileHeight;
	std::vector<TileSet> tilesets;

	// Add a vector of MapLayer to MapData
	std::vector<MapLayer> layers;
};

class Map : public Module
{
public:

	Map();

	// Destructor
	virtual ~Map();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before rendering, after all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(std::string path, std::string mapFileName);

	// Translate tile coordinates (i, j) into world (pixel) coordinates
	Vector2D MapToWorld(int i, int j) const;

	// Return the tileset a gid belongs to, nullptr if none match
	const TileSet* GetTilesetFromTileId(int gid) const;

	// Parse a <properties> node's <property> children into properties
	bool LoadProperties(const pugi::xml_node& node, Properties& properties);

	// Return the map's total size in pixels (width/height in
	// tiles times tile size) -- used to clamp the camera to the map's edges
	Vector2D GetMapSizeInPixels() const;

public:
	std::string mapFileName;
	std::string mapPath;

private:
	bool mapLoaded;

	// Declare a MapData member to hold the parsed map
	MapData mapData;
};
