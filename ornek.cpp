class AbstractMemory
{
public:
    virtual bool write(size_t address, size_t length, const uint8_t *data) = 0;
    virtual bool read(size_t address, size_t length, uint8_t *data) = 0;
    virtual size_t size() = 0;
};

// Datadan id'ye erişim sağlayan sınıf
class InformationOrnek
{
public:
    using Identifier = uint32_t;
    constexpr int header_lenght = 10;
    Identifier identify(std::vector<uint8_t> &data)
    {
        // return something;
    }
};

template <typename Information>
class Tablo
{
private:
    AbstractMemory *memory;

public:
    Tablo(AbstractMemory *memory)
        : memory(memory){};
    Tablo(const Tablo &other) = delete;
    Tablo &operator=(const Tablo &other) = delete;
    ~Tablo();

    bool store(std::vector<uint8_t> &data);                               // yer yoksa hata donecek
    std::optional<std::vector<uint8_t>> load(Information::Identifier id); // id yoksa {} doner
    bool std::optional<std::vector<Information::Identifier>> list();      // bellekteki tum id'ler, hata olursa {} doner
    bool delete (Information::Identifier id);                             // id yoksa veya memorye yazamazsa hata donecek
    bool defragment();                                                    // bellekteki bosluklari birlestirir
    std::optional<size_t> freeSize();                                     // bos alani doner
    bool initialize();                                                    // bellek icini ilk kullanim icin sifirlar
    bool isInitialized();                                                 // bellek ilklendirilmis mi?
    // (bellegin basina ilklendirilip ilklenmedigini anlamak icin bir pattern yazabiliriz ornegin)
    bool refresh();                                                       // bellegi tekrar okuyup kendi tablolarini guncelleyecek
};
