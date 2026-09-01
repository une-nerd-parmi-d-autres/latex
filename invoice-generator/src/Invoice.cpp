#include "invoice/Invoice.hpp"

#include <fstream>
#include <nlohmann/json.hpp>

namespace invoice
{

bool Invoice::loadFromJson(const std::string& filename)
{
    std::ifstream file(filename);

    if (!file)
        return false;

    nlohmann::json json;

    try
    {
        file >> json;

        company.name = json["company"]["name"];
        company.address = json["company"]["address"];
        company.phoneNumber = json["company"]["phoneNumber"];
        company.email = json["company"]["email"];

        number = json["number"];
        date = json["date"];

        customer.name = json["customer"]["name"];
        customer.address = json["customer"]["address"];

        items.clear();

        for (const auto& item : json["items"])
        {
            items.push_back({
                item["description"],
                item["quantity"],
                item["unitPrice"]
            });
        }
    }
    catch (const nlohmann::json::exception&)
    {
        return false;
    }

    return true;
}

} // namespace invoice