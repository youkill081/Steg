//
// Created by Roumite on 17/02/2026.
//

#include <iostream>

#include "src/handlers/handlers.h"
#include "src/handlers/IHander.h"
#include "src/images/Image.h"
#include "src/steganographer/Steganographer.h"
#include "src/utils/Logger.h"
#include "src/utils/Parameters.h"

int main(int ac, char** av)
{
    try
    {
        Parameters params(ac, av);
        Image image(params.get_image_path());

        if (params.get_mode() == ENCODE)
        {
            std::unique_ptr<IHandler> handler = handler_factory(params.get_type());
            ByteBuffer data = handler->encode(params);
            Steganographer::encode(image, data, params.get_type());
            image.save_png(params.get_output_path());
        }
        else if (params.get_mode() == DECODE)
        {
            Steganographer::DecodeResult result = Steganographer::decode(image);
            auto data_type = static_cast<DataType>(result.header.data_type);

            const std::unique_ptr<IHandler> handler = handler_factory(data_type);
            handler->decode(params, result.data);
        }
    }
    catch (const ParametersError &)
    {}
    catch (const std::exception &e)
    {
        Logger::log(std::string("Error -> ") + e.what());
    }

    return 42;
}
