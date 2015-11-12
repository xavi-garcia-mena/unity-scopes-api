#!/bin/sh

# Copyright (C) 2015 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Michi Henning <michi.henning@canonical.com>

#
# Script to generate debian files for dual landing in Vivid (gcc 4.9 ABI)
# and Wily and later (gcc 5 ABI).
#
# This script is called from debian/rules and generates:
#
# - control
# - libunity-scopes${soversion}.install
# - libunity-scopes${soversion}.scope.click-hook
# - shlibs.libunity-scopes-${full_version}.shlibs (for Wily and later only)
# - shlibs.libunity-scopes-qt-${qt_full_version}.shlibs (for Wily and later only)
# - libunity-scopes-qt${qt_soversion}.install
#
# For all but control, this is a straight substition and/or renaming exercise for each file.
# For control, if building on Wily or later, we also fix the "Replaces:" and "Conflicts:"
# entries, so we don't end up with two packages claiming ownership of the same places
# in the file system.
#
# Because the debian files for the different distributions are generated on the fly,
# this allows us to keep a single source tree for both distributions. See ../HACKING
# for more explanations.
#

set -e  # Fail if any command fails.

progname=$(basename $0)

[ $# -ne 1 ] && {
    echo "usage: $progname path-to-debian-dir" >&2
    exit 1
}
dir=$1
version_dir=$(mktemp -d)

# Dump version numbers into files and initialize vars from those files.

sh ${dir}/get-versions.sh ${dir} ${version_dir}

full_version=$(cat "${version_dir}"/libunity-scopes.full-version)
qt_full_version=$(cat "${version_dir}"/libunity-scopes-qt.full-version)

major_minor=$(cat "${version_dir}"/libunity-scopes.major-minor-version)
qt_major_minor=$(cat "${version_dir}"/libunity-scopes-qt.major-minor-version)

soversion=$(cat "${version_dir}"/libunity-scopes.soversion)
qt_soversion=$(cat "${version_dir}"/libunity-scopes-qt.soversion)

vivid_soversion=$(cat "${version_dir}"/libunity-scopes.vivid-soversion)

warning=$(mktemp -t gen-debian-files-msg.XXX)

trap "rm -fr $warning $version_dir" 0 INT TERM QUIT

warning_msg()
{
    cat >$warning <<EOF
# This file is autogenerated. DO NOT EDIT!
#
# Modifications should be made to $(basename "$1") instead.
# This file is regenerated automatically in the clean target.
#
EOF
}

# Generate debian/control from debian/control.in, substituting the soversion for both libs.
# For wily onwards, we also add an entry for the vivid versions to "Conflicts:" and "Replaces:".

infile="${dir}"/control.in
outfile="${dir}"/control
warning_msg $infile
cat $warning $infile \
    | sed -e "s/@UNITY_SCOPES_SOVERSION@/${soversion}/" \
          -e "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/" >"$outfile"

[ "$distro" != "vivid" ] && {
    sed -i -e "/Replaces: libunity-scopes0,/a\
\          libunity-scopes${vivid_soversion}," \
           -e "/Conflicts: libunity-scopes0,/a\
\           libunity-scopes${vivid_soversion}," \
        "$outfile"
}

# Generate the install files, naming them according to the soversion.

# Install file for binary package
infile="${dir}"/libunity-scopes.install.in
outfile="${dir}"/libunity-scopes${soversion}.install
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

# Install file for click hook
infile="${dir}"/libunity-scopes.scope.click-hook.in
outfile="${dir}"/libunity-scopes${soversion}.scope.click-hook
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

# Shlibs file
infile="${dir}"/libunity-scopes.shlibs.in
outfile="${dir}"/libunity-scopes-${full_version}.shlibs
warning_msg "$infile"
cat $warning "$infile" \
    | sed -e "s/@UNITY_SCOPES_SOVERSION@/${soversion}/g" \
          -e "s/@UNITY_SCOPES_MAJOR_MINOR@/${major_minor}/g" \
        >"$outfile"

infile="${dir}"/libunity-scopes-qt.shlibs.in
outfile="${dir}"/libunity-scopes-qt${qt_full_version}.shlibs
warning_msg "$infile"
cat $warning "$infile" \
    | sed -e "s/@UNITY_SCOPES_QT_SOVERSION@/${qt_soversion}/g" \
          -e "s/@UNITY_SCOPES_QT_MAJOR_MINOR@/${qt_major_minor}/g" \
        >"$outfile"

# Install file for qt binary package
infile="${dir}"/libunity-scopes-qt.install.in
outfile="${dir}"/libunity-scopes-qt${qt_soversion}.install
warning_msg "$infile"
cat $warning "$infile" >"$outfile"

exit 0