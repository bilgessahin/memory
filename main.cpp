#include <iostream>
#include <vector>
#include <optional>
#include <map>
#include "Utils.h"
#include <numeric>

using namespace std;

#define ADDRESS_TABLE 800 /*entry sayısı * size kadar güncellenecek*/

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
    size_t address = 4;
    size_t length = 4;
    using adress2 =  size_t;
    static constexpr int header_lenght = 10;
    Identifier identify(std::vector<uint8_t> &data)
    {
        // return something;

    }

private:

};


template <typename Information>
class Tablo
{
private:
    AbstractMemory *memory;
    InformationOrnek information;
    std::map<typename Information::Identifier, std::pair<size_t, size_t>> add_len_id_map; //id-offset-length
    std::vector<uint8_t> magic_number{0xaa, 0x55, 0x00, 0xaa};
    Utils utl;

public:
    Tablo(AbstractMemory *memory)
        : memory(memory){};
    Tablo(const Tablo &other) = delete;
    Tablo &operator=(const Tablo &other) = delete;
    ~Tablo();

    bool store(std::vector<uint8_t> &data) // yer yoksa hata donecek
    {
        /*değerleri tutmak için vektör tanımlandı. */
        std::vector< std::pair<typename Information::Identifier, std::pair<int,int>> > vec;
        vec = utl.sortfn(add_len_id_map); /*vektör mapteki adres değerlerine göre küçükten büyüğe sıralandı*/

        if(freeSize() > data.size()) /*boş alan varsa işleme devam et*/
        {
            for(int i = 0; i < vec.size(); i++) /*mapte arada boşluk varsa boşluğa yazma kısmı*/
            {
                int temp = 0;
                size_t addres = vec[i].second.first;
                size_t length = vec[i].second.second;
                temp = addres + length;
                size_t sonuc = std::abs(temp - vec[i+1].second.first);
                if(data.size() < sonuc) /*kendinden büyük ilk en küçük uzunluğu bulluyor*/
                {
                    bool response = memory->write(temp, data.size(), data.data()); /*memorye yazma*/

                    if(response) {
                        for(size_t i = magic_number.size(); i < ADDRESS_TABLE; i += (information.address + information.length)) /*Adress tablosuna yazma*/
                        {
                            std::vector<uint8_t> add_vec (information.address + information.length, 0x00);
                            std::vector<uint8_t> zero_vec (information.address + information.length, 0x00);
                            //Adres tablosundan veri okuma
                            bool result = memory->read(i, add_vec.size(), add_vec.data());
                            if(result)
                            {
                                if(add_vec == zero_vec) /*bos alan bulundu*/
                                {
                                    std::vector<uint8_t> addres_data = utl.intToByteArr(temp);
                                    std::vector<uint8_t> length_data = utl.intToByteArr(data.size());

                                    memory->write(i, information.address, addres_data.data());
                                    memory->write(i + information.address, information.length, length_data.data());

                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    std::optional<std::vector<uint8_t>> load(typename Information::Identifier id) // id yoksa {} doner
    {
        auto it = add_len_id_map.find(id);
        if(it != add_len_id_map.end())
        {
            std::vector<uint8_t> data(add_len_id_map.at(id).second, 0x00);
            bool response = memory->read(add_len_id_map.at(id).first, data.size(), data);
            if(response)
                return data;
            return {};
        }
        return {};
    }

    //Adres tablosundaki adreslerdeki veriyi okuyup information sınıfına verip id alacağım.
    std::optional<std::vector<typename Information::Identifier>> list()      // bellekteki tum id'ler, hata olursa {} doner
    {
        std::vector<uint8_t> add_vec (information.address, 0x00); // adres için data vektörü
        std::vector<typename Information::Identifier> id_vec; //id vektörü
        for(size_t i = magic_number.size(); i < ADDRESS_TABLE; i += (information.address + information.length))
        {
            //Adres tablosundan veri okuma
            bool result = memory->read(i, add_vec.size(), add_vec.data());
            if(result)
            {
                std::vector<uint8_t> data_vec (information.header_lenght, 0x00); //data vektörü
                bool response = memory->read(utl.byteArrayToInt(add_vec), information.header_lenght, data_vec.data());
                if(response){
                    auto r = information.identify(data_vec);
                    id_vec.push_back(r);
                }
                else
                    return {};
            }
            else
                return {};
        }
        return id_vec;
    }


    bool delete_Id (typename Information::Identifier id)  // id yoksa veya memorye yazamazsa hata donecek
    {
        //id'yi mapte bulma
        auto it = add_len_id_map.find(id);
        if(it != add_len_id_map.end())
        {
            //memoryden silme
            std::vector<uint8_t> delete_memory(add_len_id_map.at(id).second, 0x00);
            bool result = memory->write(add_len_id_map.at(id).first, delete_memory.size(), delete_memory);
            if(result){

                //adres table silme
                std::vector<uint8_t> data_vec (information.length, 0x00);
                for(size_t i = magic_number.size(); i < ADDRESS_TABLE; i += (information.address + information.length))
                {
                    memory->read(i, data_vec.size(), data_vec.data());
                    //okunan adres var mı?
                    if(data_vec == utl.intToByteArr(add_len_id_map.at(id).first)) //adres bulundu
                    {
                        bool result = memory->write(i, (information.address + information.length), 0x00);
                        if(result)
                        {
                            //address length ve id mapten silme
                            add_len_id_map.erase(id);
                            return true;
                        }
                        return false;
                    }
                }
                return false;
            }
            return false;
        }
        return false;
    }


    bool defragment();  // bellekteki bosluklari birlestirirz

    std::optional<size_t> freeSize()  // bos alani doner tüm alan - toplam kullanılan alan
    {
        //map içinde ki toplam lenght
        size_t total_sum = std::accumulate(add_len_id_map.begin(), add_len_id_map.end(), 0,
                                           [](const int prev_sum, const std::pair<typename Information::Identifier, std::pair<size_t,size_t>> &map) {
            return prev_sum + map.second.second;
        });

        //Toplam size - adress table size - map içinde ki toplam lenght
        return memory->size() - ADDRESS_TABLE - total_sum;
    }


    bool initialize() // bellek icini ilk kullanim icin sifirlar
    {
        //ilk 4 byte magic numberı yaz
        bool result = memory->write(0,magic_number.size(), magic_number.data());
        if(result)
        {
            //kalan memory kısmına 0 yaz
            std::vector<uint8_t> init_memory(memory->size() - magic_number.size(), 0x00);
            result = memory->write(magic_number.size(), init_memory.size(), init_memory.data()); /*magic numberdan itibaren olan alana 0 yaz*/
            if(result)
                return true;
            return false;
        }
        return false;
    }

    bool isInitialized()  // bellek ilklendirilmis mi?
    {
        size_t address = 0;
        std::vector<uint8_t> data_vec (information.length,0x00);
        memory->read(address, information.length, data_vec.data()); //memoryden ilk 4 byte oku
        if(data_vec == magic_number) // bu ilk 4 byte magic numbera eşit mi? Eşitse true ilklendirilmiştir.
            return true;
        return false;
    }
    // (bellegin basina ilklendirilip ilklenmedigini anlamak icin bir pattern yazabiliriz ornegin)
};


class AnahtarIslemciMemory : public AbstractMemory
{
    bool write(size_t address, size_t length, const uint8_t *data)
    {
        return true;
    }
    bool read(size_t address, size_t length, uint8_t *data)
    {
        return true;
    }
    size_t size()
    {
        return 1024;
    }
};


int main()
{
    cout << "Hello World!" << endl;
    return 0;
}
