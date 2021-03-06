/*
 * ml-lib, a machine learning library for Max and Pure Data
 * Copyright (C) 2013 Carnegie Mellon University
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of  MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ml_ml.h"

namespace ml
{
    const GRT::UINT default_num_hidden_neurons = 2;
    const std::string ml_object_name = "ml.mlp";
    
    typedef enum mlp_layer_
    {
        LAYER_INPUT,
        LAYER_HIDDEN,
        LAYER_OUTPUT,
        MLP_NUM_LAYERS
    }
    mlp_layer;
    
    class ml_mlp : ml
    {
        FLEXT_HEADER_S(ml_mlp, ml, setup);
        
    public:
        ml_mlp()
        :
        numHiddenNeurons(default_num_hidden_neurons),
        inputActivationFunction((GRT::Neuron::ActivationFunctions)mlp.getInputLayerActivationFunction()),
        hiddenActivationFunction((GRT::Neuron::ActivationFunctions)mlp.getHiddenLayerActivationFunction()),
        outputActivationFunction((GRT::Neuron::ActivationFunctions)mlp.getOutputLayerActivationFunction())
        {
            post("Multilayer Perceptron based on the GRT library version " + GRT::GRTBase::getGRTVersion());
            
            regression_data.setInputAndTargetDimensions(default_num_input_dimensions, default_num_output_dimensions);
            classification_data.setNumDimensions(default_num_input_dimensions);
            
            mlp.setMinChange(1.0e-2);
            set_scaling(default_scaling);
            
            std::stringstream post_stream;
            
            post_stream << "mode:\tinteger setting mode of the MLP, " << LABELLED_REGRESSION << " for regression and " <<LABELLED_CLASSIFICATION << " for classification (default " << default_data_type << ")\n";
            help.append_attributes(attribute_help);
            help.append_methods(method_help);
            help.append_attributes(post_stream.str());
        }
        
    protected:
        static void setup(t_classid c)
        {
            FLEXT_CADDMETHOD_(c, 0, "error", error);
            
            FLEXT_CADDATTR_SET(c, "mode", set_mode);
            FLEXT_CADDATTR_SET(c, "num_outputs", set_num_outputs);
            FLEXT_CADDATTR_SET(c, "num_hidden", set_num_hidden);
            FLEXT_CADDATTR_SET(c, "min_epochs", set_min_epochs);
            FLEXT_CADDATTR_SET(c, "max_epochs", set_max_epochs);
            FLEXT_CADDATTR_SET(c, "min_change", set_min_change);
            FLEXT_CADDATTR_SET(c, "training_rate", set_training_rate);
            FLEXT_CADDATTR_SET(c, "momentum", set_momentum);
            FLEXT_CADDATTR_SET(c, "gamma", set_gamma);
            FLEXT_CADDATTR_SET(c, "null_rejection", set_null_rejection);
            FLEXT_CADDATTR_SET(c, "null_rejection_coeff", set_null_rejection_coeff);
            FLEXT_CADDATTR_SET(c, "input_activation_function", set_input_activation_function);
            FLEXT_CADDATTR_SET(c, "hidden_activation_function", set_hidden_activation_function);
            FLEXT_CADDATTR_SET(c, "output_activation_function", set_output_activation_function);
            FLEXT_CADDATTR_SET(c, "rand_training_iterations", set_rand_training_iterations);
            FLEXT_CADDATTR_SET(c, "use_validation_set", set_use_validation_set);
            FLEXT_CADDATTR_SET(c, "validation_set_size", set_validation_set_size);
            FLEXT_CADDATTR_SET(c, "randomize_training_order", set_randomise_training_order);
            
            FLEXT_CADDATTR_GET(c, "mode", get_mode);
            FLEXT_CADDATTR_GET(c, "num_outputs", get_num_outputs);
            FLEXT_CADDATTR_GET(c, "num_hidden", get_num_hidden);
            FLEXT_CADDATTR_GET(c, "max_epochs", get_max_epochs);
            FLEXT_CADDATTR_GET(c, "min_change", get_min_change);
            FLEXT_CADDATTR_GET(c, "training_rate", get_training_rate);
            FLEXT_CADDATTR_GET(c, "momentum", get_momentum);
            FLEXT_CADDATTR_GET(c, "gamma", get_gamma);
            FLEXT_CADDATTR_GET(c, "null_rejection", get_null_rejection);
            FLEXT_CADDATTR_GET(c, "null_rejection_coeff", get_null_rejection_coeff);
            FLEXT_CADDATTR_GET(c, "input_activation_function", get_input_activation_function);
            FLEXT_CADDATTR_GET(c, "hidden_activation_function", get_hidden_activation_function);
            FLEXT_CADDATTR_GET(c, "output_activation_function", get_output_activation_function);
            FLEXT_CADDATTR_GET(c, "rand_training_iterations", get_rand_training_iterations);
            FLEXT_CADDATTR_GET(c, "use_validation_set", get_use_validation_set);
            FLEXT_CADDATTR_GET(c, "validation_set_size", get_validation_set_size);
            FLEXT_CADDATTR_GET(c, "randomize_training_order", get_randomise_training_order);
       
            DefineHelp(c, ml_object_name.c_str());
        }
        
        void clear();
        void train();
        void map(int argc, const t_atom *argv);
        void error();
        
        // Flext attribute setters
        void set_mode(int mode);
        void set_num_outputs(int num_outputs);
        void set_num_hidden(int num_hidden);
        void set_min_epochs(int min_epochs);
        void set_max_epochs(int max_epochs);
        void set_min_change(float min_change);
        void set_training_rate(float training_rate);
        void set_momentum(float momentum); //
        void set_gamma(float gamma);
        void set_null_rejection(bool null_rejection);
        void set_null_rejection_coeff(float null_rejection_coeff);
        void set_input_activation_function(int activation_function);
        void set_hidden_activation_function(int activation_function);
        void set_output_activation_function(int activation_function);
        void set_rand_training_iterations(int rand_training_iterations);
        void set_use_validation_set(bool use_validation_set);
        void set_validation_set_size(int validation_set_size);
        void set_randomise_training_order(bool randomise_training_order);
        
        // Flext attribute getters
        void get_mode(int &mode) const;
        void get_num_outputs(int &num_outputs) const;
        void get_num_hidden(int &num_hidden) const;
        void get_min_epochs(int &min_epochs) const;
        void get_max_epochs(int &max_epochs) const;
        void get_min_change(float &min_change) const;
        void get_training_rate(float &training_rate) const;
        void get_momentum(float &momentum) const; //
        void get_gamma(float &gamma) const;
        void get_null_rejection(bool &null_rejection) const;
        void get_null_rejection_coeff(float &null_rejection_coeff) const;
        void get_input_activation_function(int &activation_function) const;
        void get_hidden_activation_function(int &activation_function) const;
        void get_output_activation_function(int &activation_function) const;
        void get_rand_training_iterations(int &rand_training_iterations) const;
        void get_use_validation_set(bool &use_validation_set) const;
        void get_validation_set_size(int &validation_set_size) const;
        void get_randomise_training_order(bool &randomise_training_order) const;
        
        // Implement pure virtual methods
        GRT::MLBase &get_MLBase_instance();
        const GRT::MLBase &get_MLBase_instance() const;
        bool read_specialised_dataset(std::string &path);
        bool write_specialised_dataset(std::string &path) const;
        
    private:
        void set_activation_function(int activation_function, mlp_layer layer);
        
        // Flext method wrappers
        FLEXT_CALLBACK(error);

        // Flext attribute wrappers
        FLEXT_CALLVAR_I(get_mode, set_mode);
        FLEXT_CALLVAR_I(get_num_outputs, set_num_outputs);
        FLEXT_CALLVAR_I(get_num_hidden, set_num_hidden);
        FLEXT_CALLVAR_I(get_min_epochs, set_min_epochs);
        FLEXT_CALLVAR_I(get_max_epochs, set_max_epochs);
        FLEXT_CALLVAR_F(get_min_change, set_min_change);
        FLEXT_CALLVAR_F(get_training_rate, set_training_rate);
        FLEXT_CALLVAR_F(get_momentum, set_momentum);
        FLEXT_CALLVAR_F(get_gamma, set_gamma);
        FLEXT_CALLVAR_B(get_null_rejection, set_null_rejection);
        FLEXT_CALLVAR_F(get_null_rejection_coeff, set_null_rejection_coeff);
        FLEXT_CALLVAR_I(get_input_activation_function, set_input_activation_function);
        FLEXT_CALLVAR_I(get_hidden_activation_function, set_hidden_activation_function);
        FLEXT_CALLVAR_I(get_output_activation_function, set_output_activation_function);
        FLEXT_CALLVAR_I(get_rand_training_iterations, set_rand_training_iterations);
        FLEXT_CALLVAR_B(get_use_validation_set, set_use_validation_set);
        FLEXT_CALLVAR_I(get_validation_set_size, set_validation_set_size);
        FLEXT_CALLVAR_B(get_randomise_training_order, set_randomise_training_order);

        // Virtual method override
        virtual const std::string get_object_name(void) const { return ml_object_name; };
        
        GRT::MLP mlp;
        GRT::UINT numHiddenNeurons;
        GRT::Neuron::ActivationFunctions inputActivationFunction;
        GRT::Neuron::ActivationFunctions hiddenActivationFunction;
        GRT::Neuron::ActivationFunctions outputActivationFunction;
        
        static const std::string method_help;
        static const std::string attribute_help;
    };
    
    // Flext attribute setters
    void ml_mlp::set_mode(int mode)
    {
        if (mode > MLP_NUM_DATA_TYPES)
        {
            flext::error("mode must be between 0 and %d", MLP_NUM_DATA_TYPES - 1);
            return;
        }
        
        if (mode == LABELLED_CLASSIFICATION)
        {
            set_num_outputs(1);
        }
        
        set_data_type((ml_data_type)mode);
    }
    
    void ml_mlp::set_num_outputs(int num_outputs)
    {
        int curr_num_outputs = 0;
        
        get_num_outputs(curr_num_outputs);
        
        if (num_outputs == curr_num_outputs)
        {
            return;
        }
        
        const ml_data_type data_type = get_data_type();
        
        if (num_outputs < 0)
        {
            flext::error("number of outputs must be greater than zero");
        }
        
        if (data_type == LABELLED_CLASSIFICATION && num_outputs > 1)
        {
            flext::error("for classification mode, number of outputs must be 1, for multidimensional output switch mode to %d", LABELLED_REGRESSION);
            return;
        }
        
        if (data_type == LABELLED_REGRESSION)
        {
            bool success = regression_data.setInputAndTargetDimensions(regression_data.getNumInputDimensions(), num_outputs);
           
            if (success == false)
            {
                flext::error("unable to set input and target dimensions");
                return;
            }
        }
    }
    
    void ml_mlp::set_num_hidden(int num_hidden)
    {
        this->numHiddenNeurons = num_hidden;
    }
    
    void ml_mlp::set_min_epochs(int min_epochs)
    {
        bool success = mlp.setMinNumEpochs(min_epochs);
        
        if (success == false)
        {
            flext::error("unable to set min_epochs, hint: should be greater than 0");
        }
    }

    void ml_mlp::set_max_epochs(int max_epochs)
    {
        mlp.setMaxNumEpochs(max_epochs);
    }
    
    void ml_mlp::set_min_change(float min_change)
    {
        bool success = mlp.setMinChange(min_change);
        
        if (success == false)
        {
            flext::error("unable to set min_change, hint: should be greater than 0");
        }
    }
    
    void ml_mlp::set_training_rate(float training_rate)
    {
        bool success = mlp.setTrainingRate(training_rate);
        
        if (success == false)
        {
            flext::error("unable to set training_rate, hint: should be between 0-1");
        }
    }
    
    void ml_mlp::set_momentum(float momentum)
    {
        bool success = mlp.setMomentum(momentum);
        
        if (success == false)
        {
            flext::error("unable to set momentum, hint: should be between 0-1");
        }
    }
    
    void ml_mlp::set_gamma(float gamma)
    {
        bool success = mlp.setGamma(gamma);
        
        if (success == false)
        {
            flext::error("unable to set gamma");
        }
    }

    void ml_mlp::set_null_rejection(bool null_rejection)
    {
        bool success = mlp.setNullRejection(null_rejection);
        
        if (success == false)
        {
            flext::error("unable to set null_rejection");
        }
    }

    void ml_mlp::set_null_rejection_coeff(float null_rejection_coeff)
    {
        bool success = mlp.setNullRejectionCoeff(null_rejection_coeff);
        
        if (success == false)
        {
            flext::error("unable to set null_rejection_coeff, hint: should be greater than 0");
        }
    }
    
    void ml_mlp::set_activation_function(int activation_function, mlp_layer layer)
    {
        if (mlp.validateActivationFunction(activation_function) == false)
        {
            flext::error("activation function %d is invalid, hint should be between 0-%d", activation_function, GRT::Neuron::NUMBER_OF_ACTIVATION_FUNCTIONS - 1);
            return;
        }
        
        GRT::Neuron::ActivationFunctions activation_function_ = (GRT::Neuron::ActivationFunctions)activation_function;
        
        switch (layer)
        {
            case LAYER_INPUT:
                inputActivationFunction = activation_function_;
                break;
            case LAYER_HIDDEN:
                hiddenActivationFunction = activation_function_;
                break;
            case LAYER_OUTPUT:
                outputActivationFunction = activation_function_;
                break;
            default:
                ml::error("no activation function for layer: " + std::to_string(layer));
                return;
        }
        post("activation function set to " + mlp.activationFunctionToString(activation_function_));
    }
    
    void ml_mlp::set_input_activation_function(int activation_function)
    {
        set_activation_function(activation_function, LAYER_INPUT);
    }
    
    void ml_mlp::set_hidden_activation_function(int activation_function)
    {
        set_activation_function(activation_function, LAYER_HIDDEN);
    }
    
    void ml_mlp::set_output_activation_function(int activation_function)
    {
        set_activation_function(activation_function, LAYER_OUTPUT);
    }
    
    void ml_mlp::set_rand_training_iterations(int rand_training_iterations)
    {
        bool success = mlp.setNumRandomTrainingIterations(rand_training_iterations);
        
        if (success == false)
        {
            flext::error("unable to set rand_training_iterations, hint: should be greater than 0");
        }
    }
    
    void ml_mlp::set_use_validation_set(bool use_validation_set)
    {
        bool success = mlp.setUseValidationSet(use_validation_set);
        
        if (success == false)
        {
            flext::error("unable to set use_validation_set, hint: should be 0 or 1");
        }
    }
    
    void ml_mlp::set_validation_set_size(int validation_set_size)
    {
        bool success = mlp.setValidationSetSize(validation_set_size);
        
        if (success == false)
        {
            flext::error("unable to set validation_set_size, hint: should be between 0-100");
        }
    }

    void ml_mlp::set_randomise_training_order(bool randomise_training_order)
    {
        bool success = mlp.setRandomiseTrainingOrder(randomise_training_order);
        
        if (success == false)
        {
            flext::error("unable to set randomise_training_order, hint: should be 0 or 1");
        }
    }
    
    // Flext attribute getters
    void ml_mlp::get_mode(int &mode) const
    {
        mode = get_data_type();
    }
    
    void ml_mlp::get_num_outputs(int &num_outputs) const
    {
        const ml_data_type data_type = get_data_type();
        
        if (data_type == LABELLED_CLASSIFICATION)
        {
            num_outputs = default_num_output_dimensions;
        }
        else if (data_type == LABELLED_REGRESSION)
        {
            num_outputs = regression_data.getNumTargetDimensions();
        }
    }
    
    void ml_mlp::get_num_hidden(int &num_hidden) const
    {
        num_hidden = this->numHiddenNeurons;
    }
    
    void ml_mlp::get_min_epochs(int &min_epochs) const
    {
        min_epochs = mlp.getMaxNumEpochs();
    }
    
    void ml_mlp::get_max_epochs(int &max_epochs) const
    {
        max_epochs = mlp.getMaxNumEpochs();
    }

    void ml_mlp::get_min_change(float &min_change) const
    {
        flext::error("function not implemented");
    }
    
    void ml_mlp::get_training_rate(float &training_rate) const
    {
        training_rate = mlp.getTrainingRate();
    }
    
    void ml_mlp::get_momentum(float &momentum) const
    {
        momentum = mlp.getMomentum();
    }
    
    void ml_mlp::get_gamma(float &gamma) const
    {
        gamma = mlp.getGamma();
    }
    
    void ml_mlp::get_null_rejection(bool &null_rejection) const
    {
        null_rejection = mlp.getNullRejectionEnabled();
    }
    
    void ml_mlp::get_null_rejection_coeff(float &null_rejection_coeff) const
    {
        null_rejection_coeff = mlp.getNullRejectionCoeff();
    }
    
    void ml_mlp::get_input_activation_function(int &activation_function) const
    {
        activation_function = inputActivationFunction;
    }
    
    void ml_mlp::get_hidden_activation_function(int &activation_function) const
    {
        activation_function = hiddenActivationFunction;
    }
    
    void ml_mlp::get_output_activation_function(int &activation_function) const
    {
        activation_function = outputActivationFunction;
    }
    
    void ml_mlp::get_rand_training_iterations(int &rand_training_iterations) const
    {
        rand_training_iterations = mlp.getNumRandomTrainingIterations();
    }

    void ml_mlp::get_use_validation_set(bool &use_validation_set) const
    {
        flext::error("function not implemented");
    }
    
    void ml_mlp::get_validation_set_size(int &validation_set_size) const
    {
        validation_set_size = mlp.getValidationSetSize();
    }
    
    void ml_mlp::get_randomise_training_order(bool &randomise_training_order) const
    {
        flext::error("function not implemented");
    }
    
    // Methods
    // NOTE: MLP is special since it supports both regression and classification, we therefore override these methods
    void ml_mlp::train()
    {
        const ml_data_type data_type = get_data_type();
        
        GRT::UINT numSamples = data_type == LABELLED_CLASSIFICATION ? classification_data.getNumSamples() : regression_data.getNumSamples();
        
        if (numSamples == 0)
        {
            flext::error("no observations added, use 'add' to add training data");
            return;
        }
        
        bool success = false;
        
        if (data_type == LABELLED_CLASSIFICATION)
        {
            mlp.init(
                     classification_data.getNumDimensions(),
                     numHiddenNeurons,
                     classification_data.getNumClasses(),
                     inputActivationFunction,
                     hiddenActivationFunction,
                     outputActivationFunction
                     );
            success = mlp.train(classification_data);
        }
        else if (data_type == LABELLED_REGRESSION)
        {
            mlp.init(
                     regression_data.getNumInputDimensions(),
                     numHiddenNeurons,
                     regression_data.getNumTargetDimensions(),
                     inputActivationFunction,
                     hiddenActivationFunction,
                     outputActivationFunction
                     );
            success = mlp.train(regression_data);
        }

        if (!success)
        {
            flext::error("training failed");
        }
        
        t_atom a_success;
        
        SetInt(a_success, success);
        ToOutAnything(1, s_train, 1, &a_success);
    }
    
    void ml_mlp::clear()
    {
        mlp.clear();
        ml::clear();
    }
        
    void ml_mlp::map(int argc, const t_atom *argv)
    {
        const ml_data_type data_type = get_data_type();

        GRT::UINT numSamples = data_type == LABELLED_CLASSIFICATION ? classification_data.getNumSamples() : regression_data.getNumSamples();

        if (numSamples == 0)
        {
            flext::error("no observations added, use 'add' to add training data");
            return;
        }

        if (mlp.getTrained() == false)
        {
            flext::error("model has not been trained, use 'train' to train the model");
            return;
        }
        
        GRT::UINT numInputNeurons = mlp.getNumInputNeurons();
        GRT::VectorDouble query(numInputNeurons);
        
        if (argc < 0 || (unsigned)argc != numInputNeurons)
        {
            flext::error("invalid input length, expected %d, got %d", numInputNeurons, argc);
        }

        for (uint32_t index = 0; index < (uint32_t)argc; ++index)
        {
            double value = GetAFloat(argv[index]);
            query[index] = value;
        }
        
        bool success = mlp.predict(query);
        
        if (success == false)
        {
            flext::error("unable to map input");
            return;
        }
        
        // TODO: add probs to attributes
        if (mlp.getClassificationModeActive())
        {
            GRT::VectorDouble likelihoods = mlp.getClassLikelihoods();
            GRT::vector<GRT::UINT> labels = classification_data.getClassLabels();
            GRT::UINT classification = mlp.getPredictedClassLabel();
            
            if (likelihoods.size() != labels.size())
            {
                flext::error("labels / likelihoods size mismatch");
            }
            else
            {
                AtomList probs;

                for (uid_t count = 0; count < labels.size(); ++count)
                {
                    t_atom label_a;
                    t_atom likelihood_a;
                    
                    // Need to call SetDouble() first or label_a gets corrupted. Bug in Flext?
                    SetDouble(&likelihood_a, likelihoods[count]);
                    SetInt(label_a, labels[count]);
                    
                    probs.Append(label_a);
                    probs.Append(likelihood_a);
                }
                ToOutAnything(1, s_probs, probs);
            }
                 
            ToOutInt(0, classification);
        }
        else if (mlp.getRegressionModeActive())
        {
            GRT::VectorDouble regression_data = mlp.getRegressionData();
            GRT::VectorDouble::size_type numOutputDimensions = regression_data.size();
            
            if (numOutputDimensions != mlp.getNumOutputNeurons())
            {
                flext::error("invalid output dimensions: %d", numOutputDimensions);
                return;
            }
            
            AtomList result;
            
            for (uint32_t index = 0; index < numOutputDimensions; ++index)
            {
                t_atom value_a;
                double value = regression_data[index];
                SetFloat(value_a, value);
                result.Append(value_a);
            }
            
            ToOutList(0, result);
        }
    }
    
    // Methods
    
    void ml_mlp::error()
    {
        if (!mlp.getTrained())
        {
            flext::error("model not yet trained, send the \"train\" message to train");
            return;
        }
                
        float error_f = mlp.getTrainingError();
        t_atom error_a;
        
        SetFloat(error_a, error_f);
        
        ToOutAnything(0, s_error, 1, &error_a);
                      
    }
    
    // Implement pure virtual methods
    GRT::MLBase &ml_mlp::get_MLBase_instance()
    {
        return mlp;
    }
    
    const GRT::MLBase &ml_mlp::get_MLBase_instance() const
    {
        return mlp;
    }
    
    bool ml_mlp::read_specialised_dataset(std::string &path)
    {
        bool success = false;
        
        success = classification_data.loadDatasetFromFile(path);
        
        if (success)
        {
            set_data_type(LABELLED_CLASSIFICATION);
            return success;
        }
        
        success = regression_data.loadDatasetFromFile(path);
        
        if (success)
        {
            set_data_type(LABELLED_REGRESSION);
        }
        
        return success;
        
    }
    
    bool ml_mlp::write_specialised_dataset(std::string &path) const
    {
        const ml_data_type data_type = get_data_type();

        if (data_type == LABELLED_CLASSIFICATION)
        {
            return classification_data.saveDatasetToFile(path);
        }
        else if (data_type == LABELLED_REGRESSION)
        {
            return regression_data.saveDatasetToFile(path);
        }
        
        flext::error("unable to write dataset, invalid data type: %d", data_type);
        
        return false;
    }
    
    const std::string ml_mlp::method_help =
    "add:\tlist comprising a class id followed by n features; <class> <feature 1> <feature 2> etc when in classification mode or N output values followed by M input values when in regression mode, where N is determined by the num_outputs attribute\n"
    "write:\twrite training examples, first argument gives path to write location\n"
    "read:\tread training examples, first argument gives path to the read location\n"
    "train:\ttrain the MLP based on vectors added with 'add'\n"
    "clear:\tclear the stored training data and model\n"
    "map:\tgive the class of the input feature vector provided as a list in classification mode or the regression outputs in regression mode\n"
    "help:\tpost this usage statement to the console\n";
    const std::string ml_mlp::attribute_help =
    "num_outputs:\tinteger setting number of neurons in the output layer of the MLP (default " + std::to_string( default_num_output_dimensions) + ")\n"
    "num_hidden:\tinteger setting number of neurons in the hidden layer of the MLP (default " + std::to_string( default_num_hidden_neurons) + ")\n"
    "min_epochs:\tinteger setting the minimum number of training iterations (default 10)\n"
    "max_epochs:\tinteger setting the maximum number of training iterations (default 100)\n"
    "min_change:\tfloating point value setting the minimum change that must be achieved between two training epochs for the training to continue (default 1.0e-5)\n"
    "training_rate:\tfloating point value used to update the weights at each step of the stochastic gradient descent (default 0.1)\n"
    "momentum:\tfloating point value setting the momentum of the MLP (default 0.5)\n"
    "gamma:\tfloating point value setting the gamma of the MLP (default 2.0)\n"
    "null_rejection:\tinteger (0 or 1) toggling NULL rejection off or on, when 'on' classification results below the NULL-rejection threshold will be discarded (default 1)\n"
    "null_rejection_coeff:\tfloating point value setting a multiplier for the NULL-rejection threshold (default 0.9)\n"
    "input_activation_function:\tinteger determining the activation function for the input layer, 0:LINEAR, 1:SIGMOID, 2:BIPOLAR_SIGMOID (default LINEAR)\n"
    "hidden_activation_function:\tinteger determining the activation function for the hidden layer, 0:LINEAR, 1:SIGMOID, 2:BIPOLAR_SIGMOID (default LINEAR)\n"
    "output_activation_function:\tinteger determining the activation function for the output layer, 0:LINEAR, 1:SIGMOID, 2:BIPOLAR_SIGMOID (default LINEAR)\n"
    "rand_training_iterations:\tinteger setting the number of random training iterations (default 10)\n"
    "use_validation_set:\tinteger (0 or 1) sets whether to use a validation training set (default 1)\n"
    "validation_set_size:\tinteger integer determining the size of the validation set (default 20)\n"
    "randomize_training_order:\tinteger (0 or 1) sets whether to randomize the training order (default 0)\n"
    "scaling:\tinteger (0 or 1) sets whether values are automatically scaled (default 1)\n";
    
    typedef class ml_mlp ml0x2emlp;
    
#ifdef BUILD_AS_LIBRARY
    FLEXT_LIB(ml_object_name.c_str(), ml_mlp);
#else
    FLEXT_NEW(ml_object_name.c_str(), ml0x2emlp);
#endif
    
} //namespace ml

