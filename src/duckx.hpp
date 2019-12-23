/*
 * Under MIT license
 * Author: Amir Mohamadi (@amiremohamadi)
 * DuckX is a free library to work with docx files.
*/

#ifndef DUCKX_H
#define DUCKX_H

#include <stdlib.h>
#include <cstdio>
#include <string>

#include "pugixml.hpp"
#include "zip.h"


// TODO: Use container-iterator design pattern!

namespace duckx {
    // Run contains runs in a paragraph
    class Run {
    private:
        friend class IteratorHelper;
        // Store the parent node (a paragraph)
        pugi::xml_node parent;
        // And store current node also
        pugi::xml_node current;

    public:
        Run();
        Run(pugi::xml_node, pugi::xml_node);
        void set_parent(pugi::xml_node);
        void set_current(pugi::xml_node);

        std::string get_text() const;
        bool set_text(const std::string&) const;
        bool set_text(const char *) const;

        Run &next();
        bool has_next() const;

        enum FormattingFlags {
            None          = 0,
            Bold          = 1 << 0,
            Italic        = 1 << 1,
            Underline     = 1 << 2,
            Strikethrough = 1 << 3,
            Superscript   = 1 << 4,
            Subscript     = 1 << 5,
            SmallCaps     = 1 << 6,
            Shadow        = 1 << 7
        };
    };
    
    inline duckx::Run::FormattingFlags operator|(duckx::Run::FormattingFlags a, duckx::Run::FormattingFlags b) {
        return a = static_cast<duckx::Run::FormattingFlags>(static_cast<int>(a) | static_cast<int>(b));
    }
    
    inline duckx::Run::FormattingFlags operator&(duckx::Run::FormattingFlags a, duckx::Run::FormattingFlags b) {
        return a = static_cast<duckx::Run::FormattingFlags>(static_cast<int>(a) & static_cast<int>(b));
    }
    
    inline duckx::Run::FormattingFlags& operator|=(duckx::Run::FormattingFlags& a, duckx::Run::FormattingFlags b) {
        return a = static_cast<duckx::Run::FormattingFlags>(static_cast<int>(a) | static_cast<int>(b));
    }
    
    inline duckx::Run::FormattingFlags& operator&=(duckx::Run::FormattingFlags& a, duckx::Run::FormattingFlags b) {
        return a = static_cast<duckx::Run::FormattingFlags>(static_cast<int>(a) & static_cast<int>(b));
    }

    // Paragraph contains a paragraph
    // and stores runs
    class Paragraph {
    private:
        friend class IteratorHelper;
		// Store parent node (usually the body node)
        pugi::xml_node parent;
        // And store current node also
        pugi::xml_node current;
        // A paragraph consists of runs
        Run run;

    public:
        Paragraph();
        Paragraph(pugi::xml_node, pugi::xml_node);
        void set_parent(pugi::xml_node);
        void set_current(pugi::xml_node);

        Paragraph &next();
        bool has_next() const;

        Run &runs();
        Run &add_run(const std::string&, Run::FormattingFlags = Run::None);
        Run &add_run(const char*, Run::FormattingFlags = Run::None);
        Paragraph &insert_paragraph_after(const std::string&, Run::FormattingFlags = Run::None);
    };

	// TableCell contains one or more paragraphs
	class TableCell {
	private:
        friend class IteratorHelper;
		pugi::xml_node parent;
		pugi::xml_node current;

		Paragraph paragraph;
	public:
		TableCell();
		TableCell(pugi::xml_node, pugi::xml_node);

		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		Paragraph& paragraphs();

		TableCell& next();
		bool has_next() const;
	};

	// TableRow consists of one or more TableCells
	class TableRow {
        friend class IteratorHelper;
		pugi::xml_node parent;
		pugi::xml_node current;

		TableCell cell;
	public:
		TableRow();
		TableRow(pugi::xml_node, pugi::xml_node);
		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		TableCell& cells();

		bool has_next() const;
		TableRow& next();
	};

	// Table consists of one or more TableRow objects
	class Table {
	private:
        friend class IteratorHelper;
		pugi::xml_node parent;
		pugi::xml_node current;

		TableRow row;
	public:
		Table();
		Table(pugi::xml_node, pugi::xml_node);
		void set_parent(pugi::xml_node);
		void set_current(pugi::xml_node);

		Table& next();
		bool has_next() const;

		TableRow& rows();
	};

    // Document contains whole the docx file
    // and stores paragraphs
    class Document {
    private:
        friend class IteratorHelper;
        std::string directory;
        Paragraph paragraph;
		Table table;
        pugi::xml_document document;

    public:
        Document();
        Document(std::string);
        void file(std::string);
        void open();
        void save() const;

        Paragraph &paragraphs();
		Table& tables();
    };
}

#endif
