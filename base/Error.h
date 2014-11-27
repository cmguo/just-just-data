// Error.h

#ifndef _JUST_DATA_BASE_ERROR_H_
#define _JUST_DATA_BASE_ERROR_H_

namespace just
{
    namespace data
    {

        namespace error {

            enum errors
            {
                bad_media_type = 1, 
                no_more_segment, 
                at_end_point, 
            };

            namespace detail {
                class data_category
                    : public boost::system::error_category
                {
                public:
                    data_category()
                    {
                        register_category(*this);
                    }

                    const char* name() const
                    {
                        return "data";
                    }

                    std::string message(int value) const
                    {
                        switch (value) 
                        {
                            case bad_media_type:
                                return "data: bad media type";
                            case no_more_segment:
                                return "data: no more segment";
                            case at_end_point:
                                return "data: at end point, write complete";
                            default:
                                return "data: other error";
                        }
                    }
                };

            } // namespace detail

            inline const boost::system::error_category & get_category()
            {
                static detail::data_category instance;
                return instance;
            }

            inline boost::system::error_code make_error_code(
                errors e)
            {
                return boost::system::error_code(
                    static_cast<int>(e), get_category());
            }

        } // namespace data_error

    } // namespace data
} // namespace just

namespace boost
{
    namespace system
    {

        template<>
        struct is_error_code_enum<just::data::error::errors>
        {
            BOOST_STATIC_CONSTANT(bool, value = true);
        };

#ifdef BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
        using just::data::data_error::make_error_code;
#endif

    }
}

#endif // _JUST_DATA_BASE_ERROR_H_
