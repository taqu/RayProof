#ifndef INC_CPPOBJ_H_
#define INC_CPPOBJ_H_
// define CPPOBJ_IMPLEMENTATION before including cppobj.h to obtain implementation
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <filesystem>

namespace cppobj
{
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;

using f32 = float;
using f64 = double;

struct Vector2
{
    f32 x_;
    f32 y_;
};

struct Vector3
{
    f32 x_;
    f32 y_;
    f32 z_;
};

struct Vertex
{
    Vector3 position_;
    Vector2 texcoord_;
    Vector3 normal_;
};

template<class T>
class Array
{
public:
    inline static constexpr u32 Expand = 1024;
    Array();
    ~Array();
    u32 capacity() const;
    u32 size() const;
    void clear();
    void push_back(const T& x);
    const T& operator[](u32 index) const;
    T& operator[](u32 index);

    void resize(u32 size);
private:
    Array(const Array&) = delete;
    Array* operator=(const Array&) = delete;
    void expand(u32 capacity);
    u32 capacity_;
    u32 size_;
    T* items_;
};

template<class T>
Array<T>::Array()
    : capacity_(0)
    , size_(0)
    , items_(nullptr)
{
}

template<class T>
Array<T>::~Array()
{
    delete[] items_;
    items_ = nullptr;
}

template<class T>
u32 Array<T>::capacity() const
{
    return capacity_;
}

template<class T>
u32 Array<T>::size() const
{
    return size_;
}
template<class T>
void Array<T>::clear()
{
    size_ = 0;
}

template<class T>
void Array<T>::push_back(const T& x)
{
    if(capacity_ <= size_) {
        expand(capacity_+Expand);
    }
    items_[size_] = x;
    ++size_;
}

template<class T>
const T& Array<T>::operator[](u32 index) const
{
    assert(index < size_);
    return items_[index];
}

template<class T>
T& Array<T>::operator[](u32 index)
{
    assert(index < size_);
    return items_[index];
}

template<class T>
void Array<T>::resize(u32 size)
{
    if(capacity_ <= size) {
        u32 capacity = capacity_ + Expand;
        while(capacity<size){
            capacity += Expand;
        }
        expand(capacity);
    }
    size_ = size;
}


template<class T>
void Array<T>::expand(u32 capacity)
{
    T* items = new T[capacity];
    ::memcpy(items, items_, sizeof(T) * capacity_);
    delete[] items_;
    capacity_ = capacity;
    items_ = items;
}

bool isLineFeed(s32 c);
bool isSpace(s32 c);
void chop(std::string& str);
const char* skip_space(const char* itr, const char* end);
const char* skip_notspace(const char* itr, const char* end);

const char* rskip_space(const char* itr, const char* rend);
const char* rskip_notspace(const char* itr, const char* rend);

const char* parse_vertex(s32& p, s32& t, s32& n, const char* itr, const char* end);

struct Face
{
    u32 object_;
    u32 group_;
    u32 material_;

    s32 p0_;
    s32 p1_;
    s32 p2_;

    s32 t0_;
    s32 t1_;
    s32 t2_;

    s32 n0_;
    s32 n1_;
    s32 n2_;
};

struct IndexSet
{
    s32 p_;
    s32 t_;
    s32 n_;
};

struct Material
{
    std::string name_;
    Vector3 Ka_ = {0.0f, 0.0f, 0.0f}; // ambient
    Vector3 Kd_ = {0.0f, 0.0f, 0.0f}; // diffuse
    Vector3 Ks_ = {0.0f, 0.0f, 0.0f}; // specular color
    Vector3 Ke_ = {0.0f, 0.0f, 0.0f}; // emissive
    f32 Ns_ = 0.0f; // specular index
    f32 d_ = 1.0f; // desolve
    //f32 Tr_ = 0.0f; // transparency
    f32 Ni_ = 1.0f; // refractive index

    std::string map_Ka_;
    std::string map_Kd_;
    std::string map_Ks_;
    std::string map_Ke_;
    std::string map_Ns_;
    std::string map_d_;
    std::string map_bump_;
};

struct Group
{
    std::string name_;
};

struct Object
{
    std::string name_;
};

class Parser
{
public:
    Parser();
    ~Parser();
    bool parse(const char* filename);

    u32 getNumFaces() const;
    u32 getNumVertices() const;
    u32 getNumMaterials() const;
    const Face& getFace(u32 index) const;
    const Vertex& getVertex(u32 index) const;
    const Material& getMaterial(u32 index) const;
private:
    Parser(const Parser&) = delete;
    Parser& operator=(const Parser&) = delete;
    void close();
    s32 peek();
    void skipLineFeed();
    void getline();
    bool parseline();

    bool parse_v();
    bool parse_vt();
    bool parse_vn();
    bool parse_vp();

    bool parse_o();
    bool parse_g();
    bool parse_f();

    bool parse_mtllib();
    bool parse_usemtl();

    void correct(s32& p, s32& t, s32& n);
    void gather_indices();
    void add_vertex(
    s32& p, s32& t, s32& n,
    std::vector<IndexSet>& index_sets);


    std::filesystem::path directory_;
    FILE* file_;
    std::string line_;
    Array<Vector3> positions_;
    Array<Vector2> texcoords_;
    Array<Vector3> normals_;
    Array<Vertex> vertices_;
    std::vector<Object> objects_;
    std::vector<Group> groups_;
    std::vector<Material> materials_;
    Array<Face> faces_;
    u32 currentMaterial_;
};

class MTLParser
{
public:
    MTLParser();
    ~MTLParser();
    bool parse(std::vector<Material>& materials, const char* filename);

private:
    MTLParser(const MTLParser&) = delete;
    MTLParser& operator=(const MTLParser&) = delete;
    void close();
    s32 peek();
    void skipLineFeed();
    void getline();
    bool parseline();
    bool parse_newmtl();

    bool parse_v3(Vector3& x);
    bool parse_v1(f32& x);
    bool parse_str(std::string& str);

    FILE* file_;
    std::string line_;
    std::vector<Material>* materials_;
    Material material_;
};

} // namespace cppobj
#endif // INC_CPPOBJ_H_

#ifdef CPPOBJ_IMPLEMENTATION
#include <charconv>
#include <algorithm>

namespace cppobj
{
bool isLineFeed(s32 c)
{
    return ('\r' == c || '\n' == c);
}

bool isSpace(s32 c)
{
    return (' ' == c || '\t' == c);
}

void chop(std::string& str)
{
    while(0 < str.length()) {
        if(!isSpace(str[str.length() - 1])) {
            break;
        }
        str.pop_back();
    }
    while(0 < str.length()) {
        if(!isSpace(str[0])) {
            break;
        }
        str.erase(0, 1);
    }
}

const char* skip_space(const char* itr, const char* end)
{
    while(itr != end){
        if(!isSpace(*itr)){
            break;
        }
        ++itr;
    }
    return itr;
}

const char* skip_notspace(const char* itr, const char* end)
{
    while(itr != end){
        if(isSpace(*itr)){
            break;
        }
        ++itr;
    }
    return itr;
}

const char* rskip_space(const char* itr, const char* end)
{
    while(itr != end){
        if(!isSpace(*itr)){
            break;
        }
        --itr;
    }
    return itr;
}

const char* rskip_notspace(const char* itr, const char* end)
{
    while(itr != end){
        if(isSpace(*itr)){
            break;
        }
        --itr;
    }
    return itr;
}

const char* parse_vertex(s32& p, s32& t, s32& n, const char* itr, const char* end)
{
    p = 0;
    t = 0;
    n = 0;
    itr = skip_space(itr, end);

    // position
    auto result = std::from_chars(itr, end, p);
    if(result.ec != std::errc{}) {
        return nullptr;
    }
    itr = skip_space(result.ptr, end);
    if(itr == end || '/' != itr[0]){
        return itr;
    }
    ++itr;

    // texcoord
    if(itr == end){
        return itr;
    }
    if('/' != itr[0]) {
        result = std::from_chars(itr, end, t);
        if(result.ec != std::errc{}) {
            return nullptr;
        }
        itr = skip_space(result.ptr, end);
        if(itr == end || '/' != itr[0]) {
            return itr;
        }
    }
    ++itr;

    // normal
    result = std::from_chars(itr, end, n);
    if(result.ec != std::errc{}) {
        return nullptr;
    }
    itr = result.ptr;
    return itr;
}

Parser::Parser()
    : file_(nullptr)
    , currentMaterial_(0)
{
    line_.reserve(64);
}

Parser::~Parser()
{
    close();
}

bool Parser::parse(const char* filename)
{
    assert(nullptr != filename);
    close();
    file_ = fopen(filename, "rb");
    if(nullptr == file_) {
        return false;
    }
    directory_ = std::filesystem::absolute(filename);
    directory_.remove_filename();

    positions_.clear();
    texcoords_.clear();
    normals_.clear();
    vertices_.clear();
    {
        objects_.clear();
        objects_.push_back(Object());
        groups_.clear();
        groups_.push_back(Group());
        materials_.clear();
        materials_.push_back(Material());
    }
    faces_.clear();
    currentMaterial_ = 0;
    while(0 == feof(file_)){
        getline();
        if(line_.length() <= 0) {
            continue;
        }
        parseline();
        #ifdef _DEBUG
        printf("%s\n", line_.c_str());
        #endif
    }
    fclose(file_);
    gather_indices();
    return true;
}

u32 Parser::getNumFaces() const
{
    return faces_.size();
}

u32 Parser::getNumVertices() const
{
    return vertices_.size();
}

u32 Parser::getNumMaterials() const
{
    return materials_.size();
}

const Face& Parser::getFace(u32 index) const
{
    return faces_[index];
}

const Vertex& Parser::getVertex(u32 index) const
{
    return vertices_[index];
}

const Material& Parser::getMaterial(u32 index) const
{
    return materials_[index];
}

void Parser::close()
{
    if(nullptr != file_) {
        fclose(file_);
        file_ = nullptr;
    }
}

s32 Parser::peek()
{
    assert(0 == feof(file_));
    s32 c = fgetc(file_);
    ungetc(c, file_);
    return c;
}

void Parser::skipLineFeed()
{
    do {
        s32 c = fgetc(file_);
        if(!isLineFeed(c)) {
            ungetc(c, file_);
            return;
        }
    } while(0 == feof(file_));
}

void Parser::getline()
{
    line_.clear();
    while(0 == feof(file_)) {
        s32 c = peek();
        if(isLineFeed(c)) {
            skipLineFeed();
            break;
        }
        c = fgetc(file_);
        line_.push_back(static_cast<char>(c));
    }
    chop(line_);
}

bool Parser::parseline()
{
    if(line_.starts_with("mtllib")) {
        return parse_mtllib();
    } else if(line_.starts_with("usemtl")) {
        return parse_usemtl();
    } else if(line_.starts_with("vt")) {
        return parse_vt();
    } else if(line_.starts_with("vn")) {
        return parse_vn();
    } else if(line_.starts_with("vp")) {
        return parse_vp();
    } else if(line_.starts_with("v")) {
        return parse_v();
    } else if(line_.starts_with("f")) {
        return parse_f();
    } else if(line_.starts_with("l")) {
        return true;
    } else if(line_.starts_with("o")) {
        return parse_o();
    } else if(line_.starts_with("g")) {
        return parse_g();
    } else if(line_.starts_with("s")) {
        return true;
    }
    return true;
}

bool Parser::parse_v()
{
    assert(0<line_.size() && line_.starts_with("v"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    // x
    const char* wend = skip_notspace(itr, cend);
    f64 value0;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value0); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // y
    wend = skip_notspace(itr, cend);
    f64 value1;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value1); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // z
    wend = skip_notspace(itr, cend);
    f64 value2;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value2); ec!=std::errc{}){
        return false;
    }

    // push position
    Vector3 position = {};
    position.x_ = static_cast<f32>(value0);
    position.y_ = static_cast<f32>(value1);
    position.z_ = static_cast<f32>(value2);
    positions_.push_back(position);
    return true;
}

bool Parser::parse_vt()
{
    assert(0<line_.size() && line_.starts_with("vt"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    // u
    const char* wend = skip_notspace(itr, cend);
    f64 value0;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value0); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // v
    wend = skip_notspace(itr, cend);
    f64 value1;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value1); ec!=std::errc{}){
        return false;
    }

    // push texcoord
    Vector2 texcoord = {};
    texcoord.x_ = static_cast<f32>(value0);
    texcoord.y_ = static_cast<f32>(value1);
    texcoords_.push_back(texcoord);
    return true;
}

bool Parser::parse_vn()
{
    assert(0<line_.size() && line_.starts_with("v"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    // x
    const char* wend = skip_notspace(itr, cend);
    f64 value0;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value0); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // y
    wend = skip_notspace(itr, cend);
    f64 value1;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value1); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // z
    wend = skip_notspace(itr, cend);
    f64 value2;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value2); ec!=std::errc{}){
        return false;
    }

    // push normal
    Vector3 normal = {};
    normal.x_ = static_cast<f32>(value0);
    normal.y_ = static_cast<f32>(value1);
    normal.z_ = static_cast<f32>(value2);
    normals_.push_back(normal);
    return true;
}

bool Parser::parse_vp()
{
    return true;
}

bool Parser::parse_o()
{
    assert(0<line_.size() && line_.starts_with("o"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    Object obj;
    obj.name_.assign(itr, cend);
    objects_.push_back(std::move(obj));
    return true;
}

bool Parser::parse_g()
{
    assert(0<line_.size() && line_.starts_with("g"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    Group group;
    group.name_.assign(itr, cend);
    groups_.push_back(std::move(group));
    return true;
}

bool Parser::parse_f()
{
    assert(0<line_.size() && line_.starts_with("f"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);

    s32 p0,t0,n0;
    itr = parse_vertex(p0, t0, n0, itr, cend);
    if(nullptr == itr){
        return false;
    }

    s32 p1,t1,n1;
    itr = parse_vertex(p1, t1, n1, itr, cend);
    if(nullptr == itr){
        return false;
    }

    s32 p2,t2,n2;
    itr = parse_vertex(p2, t2, n2, itr, cend);
    if(nullptr == itr){
        return false;
    }
    {
        assert(0<objects_.size());
        assert(0<groups_.size());
        correct(p0, t0, n0);
        correct(p1, t1, n1);
        correct(p2, t2, n2);
        Face face;
        face.object_ = static_cast<u32>(objects_.size()-1);
        face.group_ = static_cast<u32>(groups_.size()-1);
        face.material_ = currentMaterial_;
        face.p0_ = p0; face.t0_ = t0; face.n0_ = n0;
        face.p1_ = p1; face.t1_ = t0; face.n1_ = n1;
        face.p2_ = p2; face.t2_ = t0; face.n2_ = n2;
        faces_.push_back(face);
    }
    for(;;){
        p1 = p2;
        t1 = t2;
        n1 = n2;
        itr = parse_vertex(p2, t2, n2, itr, cend);
        if(nullptr == itr){
            break;
        }
        correct(p2, t2, n2);
        Face face;
        face.object_ = static_cast<u32>(objects_.size() - 1);
        face.group_ = static_cast<u32>(groups_.size() - 1);
        face.material_ = currentMaterial_;
        face.p0_ = p0; face.t0_ = t0; face.n0_ = n0;
        face.p1_ = p1; face.t1_ = t0; face.n1_ = n1;
        face.p2_ = p2; face.t2_ = t0; face.n2_ = n2;
        faces_.push_back(face);
    }
    return true;
}

bool Parser::parse_mtllib()
{
    assert(0<line_.size() && line_.starts_with("mtllib"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    const char* end = skip_notspace(itr, cend);
    std::string name(itr, end);
    std::filesystem::path path = directory_;
    path += name;
    name = path.string();
    MTLParser mtlParser;
    return mtlParser.parse(materials_, name.c_str());
}

bool Parser::parse_usemtl()
{
    assert(0<line_.size() && line_.starts_with("usemtl"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    const char* end = skip_notspace(itr, cend);
    std::string name(itr, end);
    if(name.length()<=0){
        return false;
    }
    for(u32 i=0; i<materials_.size(); ++i){
        if(materials_[i].name_ == name){
            currentMaterial_ = i;
            return true;
        }
    }
    return false;
}

void Parser::correct(s32& p, s32& t, s32& n)
{
    if(p<0){
        p = static_cast<s32>(positions_.size()) + p;
    }else{
        p -= 1;
    }

    if(t<0){
        t = static_cast<s32>(texcoords_.size()) + t;
    }else{
        t -= 1;
    }

    if(n<0){
        n = static_cast<s32>(normals_.size()) + n;
    }else{
        n -= 1;
    }
}

void Parser::gather_indices()
{
    u32 size = std::max(positions_.size(), texcoords_.size());
    size = std::max(size, normals_.size());

    vertices_.resize(size);
    ::memset(&vertices_[0], 0, sizeof(Vertex)*size);
    std::vector<IndexSet> index_sets(vertices_.size(), {-1,-1,-1});
    for(u32 i=0; i<faces_.size(); ++i){
        add_vertex(faces_[i].p0_, faces_[i].t0_, faces_[i].n0_, index_sets);
        add_vertex(faces_[i].p1_, faces_[i].t1_, faces_[i].n1_, index_sets);
        add_vertex(faces_[i].p2_, faces_[i].t2_, faces_[i].n2_, index_sets);
    }
}

void Parser::add_vertex(
    s32& p, s32& t, s32& n,
    std::vector<IndexSet>& index_sets)
{
    assert(0<=p && p<static_cast<s32>(index_sets.size()));
    if(index_sets[p].p_<0){
        index_sets[p].p_ = p;
        index_sets[p].t_ = t;
        index_sets[p].n_ = n;
        vertices_[p].position_ = positions_[p];
        if(0<=t){
            vertices_[p].texcoord_ = texcoords_[t];
        }else{
            vertices_[p].texcoord_ = {0,0};
        }
        if(0<=n){
            vertices_[p].normal_ = normals_[n];
        }else{
            vertices_[p].normal_ = {0,0,0};
        }
        return;
    }

    for(u32 i=0; i<index_sets.size(); ++i){
        if(i==p){
            continue;
        }
        if(index_sets[i].p_ == p
            && index_sets[i].t_ == t
            && index_sets[i].n_ == n){
            s32 index = static_cast<s32>(i);
        }
    }

    if(index_sets[p].p_ != p
       || index_sets[p].t_ != t
       || index_sets[p].n_ != n) {
        for(u32 i = 0; i < index_sets.size(); ++i) {
            if(i == p) {
                continue;
            }
            if(index_sets[i].p_ == p
               && index_sets[i].t_ == t
               && index_sets[i].n_ == n) {
                p = t = n = static_cast<s32>(i);
                return;
            }
        }

        assert(vertices_.size() == index_sets.size());
        s32 index = static_cast<s32>(vertices_.size());
        IndexSet newSets = {index, index, index};
        Vertex vertex;
        vertex.position_ = positions_[p];
        if(0<=t){
            vertex.texcoord_ = texcoords_[t];
        }else{
            vertex.texcoord_ = {0,0};
        }
        if(0<=n){
            vertex.normal_ = normals_[n];
        }else{
            vertex.normal_ = {0,0,0};
        }
        p = t = n = index;
        index_sets.push_back(newSets);
        vertices_.push_back(vertex);
    }
}

MTLParser::MTLParser()
    : file_(nullptr)
    ,materials_(nullptr)
{
    line_.reserve(64);
}

MTLParser::~MTLParser()
{
    close();
}

bool MTLParser::parse(std::vector<Material>& materials, const char* filename)
{
    assert(nullptr != filename);
    close();
    file_ = fopen(filename, "rb");
    if(nullptr == file_) {
        return false;
    }
    materials_ = &materials;
    material_ = {};
    while(0 == feof(file_)){
        getline();
        if(line_.length() <= 0) {
            continue;
        }
        parseline();
        #ifdef _DEBUG
        printf("%s\n", line_.c_str());
        #endif
    }
    fclose(file_);
    if(0<material_.name_.length()){
        materials.push_back(std::move(material_));
    }
    return true;
}

void MTLParser::close()
{
    if(nullptr != file_) {
        fclose(file_);
        file_ = nullptr;
    }
}

s32 MTLParser::peek()
{
    assert(0 == feof(file_));
    s32 c = fgetc(file_);
    ungetc(c, file_);
    return c;
}

void MTLParser::skipLineFeed()
{
    do {
        s32 c = fgetc(file_);
        if(!isLineFeed(c)) {
            ungetc(c, file_);
            return;
        }
    } while(0 == feof(file_));
}

void MTLParser::getline()
{
    line_.clear();
    while(0 == feof(file_)) {
        s32 c = peek();
        if(isLineFeed(c)) {
            skipLineFeed();
            break;
        }
        c = fgetc(file_);
        line_.push_back(static_cast<char>(c));
    }
    chop(line_);
}

bool MTLParser::parseline()
{
    if(line_.starts_with("newmtl")) {
        return parse_newmtl();
    } else if(line_.starts_with("Ka")) {
        return parse_v3(material_.Ka_);
    } else if(line_.starts_with("Kd")) {
        return parse_v3(material_.Kd_);
    } else if(line_.starts_with("Ks")) {
        return parse_v3(material_.Ks_);
    } else if(line_.starts_with("Ke")) {
        return parse_v3(material_.Ke_);

    } else if(line_.starts_with("Ns")) {
        return parse_v1(material_.Ns_);
    } else if(line_.starts_with("d")) {
        return parse_v1(material_.d_);
    } else if(line_.starts_with("Tr")) {
        f32 Tr = 0.0f;
        if(parse_v1(Tr)){
            material_.d_ = std::clamp(1.0f-Tr, 0.0f, 1.0f);
            return true;
        }
        return false;

    } else if(line_.starts_with("map_Ka")) {
        return parse_str(material_.map_Ka_);
    } else if(line_.starts_with("map_Kd")) {
        return parse_str(material_.map_Kd_);
    } else if(line_.starts_with("map_Ks")) {
        return parse_str(material_.map_Ks_);
    } else if(line_.starts_with("map_Ke")) {
        return parse_str(material_.map_Ke_);
    } else if(line_.starts_with("map_Ns")) {
        return parse_str(material_.map_Ns_);
    } else if(line_.starts_with("map_d")) {
        return parse_str(material_.map_d_);
    } else if(line_.starts_with("map_bump")) {
        return parse_str(material_.map_bump_);
    }
    return true;
}

bool MTLParser::parse_newmtl()
{
    assert(0<line_.size() && line_.starts_with("newmtl"));
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    const char* end = skip_notspace(itr, cend);
    std::string name(itr, end);
    if(name.length()<=0){
        return false;
    }
    if(0<material_.name_.length()){
        materials_->push_back(material_);
    }
    material_ = {};
    material_.name_ = std::move(name);
    return true;
}

bool MTLParser::parse_v3(Vector3& x)
{
    assert(0<line_.size());
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    // x
    const char* wend = skip_notspace(itr, cend);
    f64 value0;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value0); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // y
    wend = skip_notspace(itr, cend);
    f64 value1;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value1); ec!=std::errc{}){
        return false;
    }
    itr = skip_space(wend, cend);

    // z
    wend = skip_notspace(itr, cend);
    f64 value2;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value2); ec!=std::errc{}){
        return false;
    }

    x.x_ = static_cast<f32>(value0);
    x.y_ = static_cast<f32>(value1);
    x.z_ = static_cast<f32>(value2);
    return true;
}

bool MTLParser::parse_v1(f32& x)
{
    assert(0<line_.size());
    const char* itr = &line_[0];
    const char* cend = itr + line_.length();
    itr = skip_notspace(itr, cend);
    itr = skip_space(itr, cend);
    // x
    const char* wend = skip_notspace(itr, cend);
    f64 value0;
    if(auto [ptr, ec] = std::from_chars(itr, wend, value0); ec!=std::errc{}){
        return false;
    }
    x = static_cast<f32>(value0);
    return true;
}

bool MTLParser::parse_str(std::string& str)
{
    assert(0<line_.length());
    const char* itr = &line_[line_.length()-1];
    const char* cend = &line_[0];
    const char* lastend = rskip_space(itr, cend);
    itr = rskip_notspace(lastend, cend) + 1;
    std::string tmp(itr, lastend+1);
    if(tmp.length()<=0){
        return false;
    }
    str = std::move(tmp);
    return true;
}
} // namespace cppobj
#endif // CPPOBJ_IMPLEMENTATION
