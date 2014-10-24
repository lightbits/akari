

struct Tilemap
{
public:
    void create(int width, int height, int *tiles = nullptr);
    bool load_from_file(string filename);
    void dispose();

    void invalidate_lut();
    void bind_lut();

    void set(int x, int y, int t);
    int  get(int x, int y);
private:
    int *_tiles;
    int _width;
    int _height;
    uint _lookup_table;
    uint _color_table;
};

void Tilemap::create(int width, int height, int *tiles = nullptr)
{
    _width = width;
    _height = height;
    _tiles = new int[width * height];
    if (tiles != nullptr)
    {
        for (int i = 0; i < width * height; i++)
            _tiles[i] = tiles[i];
    }
    else
    {
        for (int i = 0; i < width * height; i++)
            _tiles[i] = 0;
    }
    _lookup_table = gen_texture(&_tiles[0], _width, _height, GL_R32I, GL_RED_INTEGER, GL_INT, GL_NEAREST, GL_NEAREST);
}

/*
example.map
------------------------------------------------------
    texture.png     # texture name
    4 4             # width height
    0 0 0 0         # height x width rows of tile data
    0 1 2 3         
    0 1 2 3         
    3 3 3 3         
*/
bool Tilemap::load_from_file(string filename)
{

}

void Tilemap::dispose()
{
    if (_tiles != nullptr)
    {
        delete[] _tiles;
        glDeleteTextures(1, &_lookup_table);
    }
    _width = 0;
    _height = 0;
}